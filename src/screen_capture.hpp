#pragma once
#include "client_window.hpp"
#include <atomic>
#include <cstdint>
#include <optional>
#include <vector>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wil/com.h>
#include <opencv2/core.hpp>

class ScreenCapture {
public:
    enum class ROI : std::uint8_t { deck, editor, all };
    ScreenCapture(ClientWindow& client, std::atomic<bool>& running);
    std::optional<POINT> findCard(const cv::Mat& card, ROI roi = ROI::all);
private:
    bool updateFrame();
    struct MonitorDuplicator {
        HMONITOR hmonitor{};
        wil::com_ptr<IDXGIOutputDuplication> duplication;
    };
    ClientWindow& visualClient;
    std::atomic<bool>& keep_running;
    wil::com_ptr<ID3D11Device> device;
    wil::com_ptr<ID3D11DeviceContext> context;
    std::vector<MonitorDuplicator> monitors;
    wil::com_ptr<ID3D11Texture2D> cpuframe;
    D3D11_TEXTURE2D_DESC desc{};
    cv::Mat currentFrame;
    cv::Rect gameRect, deckRect, editorRect;
    POINT monitorOrigin{};
};
