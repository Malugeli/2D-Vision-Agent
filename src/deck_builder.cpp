#include "deck_builder.hpp"
#include "deck_catalog.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <print>
#include <thread>
#include <utility>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

DeckBuilder::DeckBuilder(InputController& a, ScreenCapture& v, ClientWindow& c,
                         std::atomic<bool>& running, std::filesystem::path assets)
    : bot(a), visual(v), ygo(c), keep_running(running), assetDirectory(std::move(assets)) {}

bool DeckBuilder::card_in(const Card& card) {
    if (!keep_running) return false;
    constexpr double editor_scale = 1.22;
    cv::Mat editor;
    cv::resize(card.picture, editor, {}, editor_scale, editor_scale, cv::INTER_CUBIC);
    const auto try_insert = [&] {
        const auto location = visual.findCard(editor);
        if (!location || !keep_running) return false;
        bot.drag(*location, ygo.get_UI_coordinates(UiTarget::in));
        return true;
    };
    if (try_insert()) return true;
    if (!keep_running) return false;
    bot.click(ygo.get_UI_coordinates(UiTarget::searchbar));
    bot.type_string(card.name);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    if (try_insert()) return true;
    if (!keep_running) return false;
    const POINT scrollbar = ygo.get_UI_coordinates(UiTarget::scrollbar);
    bot.mouse_move(scrollbar);
    POINT border{ygo.bounds().right, ygo.bounds().bottom};
    ClientToScreen(ygo.handle(), &border);
    while (keep_running) {
        if (try_insert()) return true;
        if (!keep_running) return false;
        POINT current{};
        if (!GetCursorPos(&current)) return false;
        const POINT next{scrollbar.x, std::min(current.y + 150, border.y - 1)};
        if (next.y <= current.y) return false;
        bot.drag(current, next);
    }
    return false;
}

bool DeckBuilder::deck_load(int choice) {
    const auto recipe = select_deck(choice);
    if (recipe.empty()) return false;
    constexpr double reference_height = 2160.0;
    for (const auto& entry : recipe) {
        if (!keep_running) return false;
        cv::Mat picture = cv::imread((assetDirectory / entry.deck_path).string());
        if (picture.empty()) {
            std::println("Kartenbild fehlt: {}", entry.deck_path);
            return false;
        }
        const double scale = ygo.bounds().bottom / reference_height;
        if (std::abs(scale - 1.0) > 0.01) {
            cv::resize(picture, picture, {}, scale, scale, cv::INTER_AREA);
        }
        const Card card{picture, entry.name};
        for (int copy = 0; copy < entry.amount; ++copy) {
            if (!card_in(card)) {
                std::println("Karte konnte nicht eingefügt werden: {}", entry.name);
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }
    if (!keep_running) return false;
    bot.click(ygo.get_UI_coordinates(UiTarget::deckname));
    bot.type_string("Maher ist King!");
    if (!keep_running) return false;
    bot.click(ygo.get_UI_coordinates(UiTarget::savedeck));
    return true;
}
