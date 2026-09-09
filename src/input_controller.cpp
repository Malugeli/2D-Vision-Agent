#include "input_controller.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <thread>

InputController::InputController(ClientWindow& window)
    : client(window), gen(std::random_device{}()) {
    inputM.type = INPUT_MOUSE;
}

void InputController::wait() {
    std::this_thread::sleep_for(std::chrono::milliseconds(
        std::max(0L, std::lround(pause(gen)))));
}

void InputController::mouse_move(POINT goal) {
    POINT start{};
    if (!GetCursorPos(&start)) return;
    POINT p1;
    POINT p2;
    POINT way;
    double distance = std::hypot((goal.x - start.x), (goal.y - start.y));
    double noise_limit = std::max(5.0, (distance * 0.15));
    double speed = 125;
    double steps = 1.0 / std::max(10.0, (distance / speed));
    std::normal_distribution<double> magnet(0, noise_limit / 3.0);
    p1.x = start.x + ((goal.x - start.x) * 0.3) + magnet(gen);
    p1.y = start.y + ((goal.y - start.y) * 0.3) + magnet(gen);
    p2.x = start.x + ((goal.x - start.x) * 0.7) + magnet(gen);
    p2.y = start.y + ((goal.y - start.y) * 0.7) + magnet(gen);
    inputM.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    for (double t = 0.0; t <= 1.0; t += steps) {
        const double u = 1 - t;
        const double tt = t * t;
        const double uu = u * u;
        const double uuu = uu * u;
        const double ttt = tt * t;
        way.x = std::lround(uuu * start.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * goal.x);
        way.y = std::lround(uuu * start.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * goal.y);
        way = client.normalize(way);
        inputM.mi.dx = way.x;
        inputM.mi.dy = way.y;
        SendInput(1, &inputM, sizeof(inputM));
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    goal = client.normalize(goal);
    inputM.mi.dx = goal.x;
    inputM.mi.dy = goal.y;
    SendInput(1, &inputM, sizeof(inputM));
}

void InputController::mouse_button(bool release) {
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = release ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(input));
    wait();
}

void InputController::click(POINT point) {
    mouse_move(point);
    mouse_button(false);
    mouse_button(true);
}

void InputController::drag(POINT start, POINT target) {
    mouse_move(start);
    mouse_button(false);
    mouse_move(target);
    mouse_button(true);
}

void InputController::key_event(BYTE key, bool release) {
    INPUT input{};
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = static_cast<WORD>(MapVirtualKeyA(key, MAPVK_VK_TO_VSC));
    input.ki.dwFlags = KEYEVENTF_SCANCODE | (release ? KEYEVENTF_KEYUP : 0);
    SendInput(1, &input, sizeof(input));
    wait();
}

void InputController::type_key(BYTE key) {
    key_event(key, false);
    key_event(key, true);
}

void InputController::type_string(std::string_view text, bool type_return) {
    for (char character : text) {
        const SHORT key = VkKeyScanA(character);
        if (key == -1 || (HIBYTE(key) & ~1) != 0)
            throw std::runtime_error("Zeichen wird vom Tastaturlayout nicht unterstützt.");
        const bool shift = (HIBYTE(key) & 1) != 0;
        if (shift) key_event(VK_SHIFT, false);
        type_key(LOBYTE(key));
        if (shift) key_event(VK_SHIFT, true);
    }
    if (type_return) type_key(VK_RETURN);
}
