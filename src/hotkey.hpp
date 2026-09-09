#pragma once
#include <Windows.h>

class Hotkey {
public:
    Hotkey(int id, UINT key);
    ~Hotkey();
    Hotkey(const Hotkey&) = delete;
    Hotkey& operator=(const Hotkey&) = delete;
private:
    int id_;
};
