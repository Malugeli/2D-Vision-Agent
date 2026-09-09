#include "client_window.hpp"
#include <algorithm>
#include <cmath>
#include <wil/result.h>

ClientWindow::ClientWindow(HWND window) : game_(window) {
    (void)bounds();
}

HWND ClientWindow::handle() const { return game_; }

RECT ClientWindow::bounds() const {
    RECT rect{};
    THROW_IF_WIN32_BOOL_FALSE(GetClientRect(game_, &rect));
    return rect;
}

POINT ClientWindow::normalize(POINT point) const {
    const auto x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const auto y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const auto width = std::max(1, GetSystemMetrics(SM_CXVIRTUALSCREEN) - 1);
    const auto height = std::max(1, GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1);
    return {std::clamp(std::lround((point.x - x) * 65535.0 / width), 0L, 65535L),
            std::clamp(std::lround((point.y - y) * 65535.0 / height), 0L, 65535L)};
}

POINT ClientWindow::get_UI_coordinates(UiTarget target) const {
    const auto rect = bounds();
    const auto factor = UI.at(std::to_underlying(target));
    POINT point{std::lround(rect.right * factor.x), std::lround(rect.bottom * factor.y)};
    THROW_IF_WIN32_BOOL_FALSE(ClientToScreen(game_, &point));
    return point;
}
