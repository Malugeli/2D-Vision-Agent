#pragma once

#include "input_controller.hpp"
#include "screen_capture.hpp"
#include <filesystem>
#include <string_view>

class DeckBuilder {
public:
    DeckBuilder(InputController& a, ScreenCapture& v, ClientWindow& c, std::atomic<bool>& running, std::filesystem::path assets);
    bool deck_load(int x);

private:
    struct Card {
        cv::Mat picture;
        std::string_view name;
    };
    bool card_in(const Card& card);
    InputController& bot;
    ScreenCapture& visual;
    ClientWindow& ygo;
    std::atomic<bool>& keep_running;
    std::filesystem::path assetDirectory;
};
