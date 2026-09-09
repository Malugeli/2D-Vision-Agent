#include "screen_capture.hpp"
#include <utility>
#include <wil/resource.h>
#include <wil/result.h>
#include <opencv2/imgproc.hpp>

ScreenCapture::ScreenCapture(ClientWindow& client, std::atomic<bool>& running)
    : visualClient(client), keep_running(running) {
    THROW_IF_FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION, &device, nullptr, &context));
    auto dxgi = device.query<IDXGIDevice>();
    wil::com_ptr<IDXGIAdapter> adapter;
    THROW_IF_FAILED(dxgi->GetAdapter(&adapter));
    for (UINT index = 0; ; ++index) {
        wil::com_ptr<IDXGIOutput> output;
        const HRESULT status = adapter->EnumOutputs(index, &output);
        if (status == DXGI_ERROR_NOT_FOUND) break;
        THROW_IF_FAILED(status);
        DXGI_OUTPUT_DESC description{};
        THROW_IF_FAILED(output->GetDesc(&description));
        if (!description.AttachedToDesktop) continue;
        MonitorDuplicator monitor{description.Monitor, {}};
        auto output1 = output.query<IDXGIOutput1>();
        THROW_IF_FAILED(output1->DuplicateOutput(device.get(), &monitor.duplication));
        monitors.push_back(std::move(monitor));
    }
}

bool ScreenCapture::updateFrame() {
    if (!keep_running) return false;
    const auto target = MonitorFromWindow(visualClient.handle(), MONITOR_DEFAULTTONEAREST);
    for (const auto& monitor : monitors) {
        if (monitor.hmonitor != target) continue;
        wil::com_ptr<IDXGIResource> frame;
        DXGI_OUTDUPL_FRAME_INFO frame_info{};
        const auto status = monitor.duplication->AcquireNextFrame(100, &frame_info, &frame);
        // Keine Suche mit veralteten Koordinaten nach einem Monitor- oder Fensterwechsel.
        if (status == DXGI_ERROR_WAIT_TIMEOUT) return false;
        THROW_IF_FAILED(status);
        const auto release = wil::scope_exit([&] { monitor.duplication->ReleaseFrame(); });
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        THROW_IF_WIN32_BOOL_FALSE(GetMonitorInfoW(monitor.hmonitor, &info));
        monitorOrigin = {info.rcMonitor.left, info.rcMonitor.top};

        auto texture = frame.query<ID3D11Texture2D>();
        D3D11_TEXTURE2D_DESC incoming{};
        texture->GetDesc(&incoming);
        if (!cpuframe || incoming.Width != desc.Width ||
            incoming.Height != desc.Height || incoming.Format != desc.Format) {
            cpuframe.reset();
            desc = incoming;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.Usage = D3D11_USAGE_STAGING;
            desc.BindFlags = 0;
            desc.MiscFlags = 0;
            THROW_IF_FAILED(device->CreateTexture2D(&desc, nullptr, &cpuframe));
        }
        context->CopyResource(cpuframe.get(), texture.get());
        D3D11_MAPPED_SUBRESOURCE mapped{};
        THROW_IF_FAILED(context->Map(cpuframe.get(), 0, D3D11_MAP_READ, 0, &mapped));
        const auto unmap = wil::scope_exit([&] { context->Unmap(cpuframe.get(), 0); });
        const auto bounds = visualClient.bounds();
        POINT start{};
        THROW_IF_WIN32_BOOL_FALSE(ClientToScreen(visualClient.handle(), &start));
        gameRect = cv::Rect(start.x - monitorOrigin.x, start.y - monitorOrigin.y,
                            bounds.right, bounds.bottom);
        const auto region = [&](UiTarget begin, UiTarget end) {
            const auto first = visualClient.get_UI_coordinates(begin);
            const auto last = visualClient.get_UI_coordinates(end);
            return cv::Rect(first.x - monitorOrigin.x, first.y - monitorOrigin.y,
                            last.x - first.x, last.y - first.y);
        };
        deckRect = region(UiTarget::deck_Begin, UiTarget::deck_End);
        editorRect = region(UiTarget::editor_Begin, UiTarget::editor_End);
        const cv::Mat bgra(desc.Height, desc.Width, CV_8UC4, mapped.pData, mapped.RowPitch);
        cv::cvtColor(bgra, currentFrame, cv::COLOR_BGRA2BGR);
        return true;
    }
    return false;
}

std::optional<POINT> ScreenCapture::findCard(const cv::Mat& card, ROI roi) {
    if (!updateFrame() || currentFrame.empty() || card.empty()) return std::nullopt;

    cv::Rect region;
    switch (roi) {
    case ROI::all: region = gameRect; break;
    case ROI::deck: region = deckRect; break;
    case ROI::editor: region = editorRect; break;
    }
    region &= cv::Rect(0, 0, currentFrame.cols, currentFrame.rows);
    if (region.width < card.cols || region.height < card.rows) return std::nullopt;

    cv::Mat result;
    cv::matchTemplate(currentFrame(region), card, result, cv::TM_CCOEFF_NORMED);
    double score{};
    cv::Point location;
    cv::minMaxLoc(result, nullptr, &score, nullptr, &location);
    if (score <= 0.7) return std::nullopt;

    return POINT{location.x + region.x + monitorOrigin.x + card.cols / 2,
                 location.y + region.y + monitorOrigin.y + card.rows / 2};
}
