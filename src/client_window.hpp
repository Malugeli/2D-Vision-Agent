#pragma once
#include <Windows.h>
#include "ui_layout.hpp"

class ClientWindow {
public:
    explicit ClientWindow(HWND window);
    HWND handle() const;
    RECT bounds() const;
    POINT normalize(POINT point) const;
    POINT get_UI_coordinates(UiTarget target) const;
private:
    HWND game_;
};
