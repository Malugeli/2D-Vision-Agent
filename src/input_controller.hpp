#pragma once
#include "client_window.hpp"
#include <random>
#include <string_view>

class InputController {
public:
    explicit InputController(ClientWindow& client);
    void mouse_move(POINT goal);
    void click(POINT point);
    void drag(POINT start, POINT target);
    void type_string(std::string_view text, bool type_return = true);
private:
    void wait();
    void key_event(BYTE key, bool release);
    void type_key(BYTE key);
    void mouse_button(bool release);
    INPUT inputM{};
    ClientWindow& client;
    std::mt19937 gen;
    std::normal_distribution<double> pause{70, 5};
};
