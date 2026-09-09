#include "hotkey.hpp"
#include <wil/result.h>

Hotkey::Hotkey(int id, UINT key) : id_(id) {
    THROW_IF_WIN32_BOOL_FALSE(RegisterHotKey(nullptr, id_, MOD_NOREPEAT, key));
}
Hotkey::~Hotkey() {
    UnregisterHotKey(nullptr, id_);
}
