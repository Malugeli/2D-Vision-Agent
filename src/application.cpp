#include "application.hpp"
#include "deck_builder.hpp"
#include "deck_selection.hpp"
#include "hotkey.hpp"
#include <atomic>
#include <chrono>
#include <exception>
#include <filesystem>
#include <future>
#include <iostream>
#include <print>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <wil/result.h>

namespace {
std::optional<int> ask_deck(int argc, char* argv[]) {
    if (argc > 2) {
        std::println("Aufruf: YgoDeckBuilder.exe [1|2]");
        return std::nullopt;
    }
    if (argc == 2) return parse_deck_choice(argv[1]);
    std::string answer;
    while (true) {
        std::println("Welches Deck? 1: Dracotail, 2: K9 Vanquish Soul");
        if (!std::getline(std::cin, answer)) return std::nullopt;
        if (auto choice = parse_deck_choice(answer)) return choice;
        std::println("Bitte 1 oder 2 eingeben.");
    }
}
std::filesystem::path asset_directory() {
    std::wstring path(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
    THROW_IF_WIN32_BOOL_FALSE(length != 0);
    if (length >= path.size()) throw std::runtime_error("Programmpfad ist zu lang.");
    path.resize(length);
    return std::filesystem::path(path).parent_path() / "Pics";
}
} // namespace

int run_application(int argc, char* argv[]) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    try {
        const auto choice = ask_deck(argc, argv);
        if (!choice) {
            std::println("Keine gültige Deckauswahl.");
            return 1;
        }
        const HWND game = FindWindowA(nullptr, "masterduel");
        if (!game) {
            std::println("Master Duel wurde nicht gefunden.");
            return 1;
        }
        std::atomic<bool> running{true};
        Hotkey stop_key(1, VK_NUMPAD0);
        ClientWindow window(game);
        InputController input(window);
        ScreenCapture capture(window, running);
        DeckBuilder builder(input, capture, window, running, asset_directory());
        std::packaged_task<bool()> task([&] { return builder.deck_load(*choice); });
        auto result = task.get_future();
        std::jthread worker(std::move(task));
        std::println("Programm startet. Numpad 0 bricht ab.");
        while (result.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
            MSG message{};
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if ((message.message == WM_HOTKEY && message.wParam == 1) ||
                    message.message == WM_QUIT) running = false;
            }
            if (!running) break;
            MsgWaitForMultipleObjects(0, nullptr, FALSE, 50, QS_ALLINPUT);
        }
        worker.join();
        const bool success = result.get();
        std::println("{}", success ? "Deck gespeichert." : "Deckbau abgebrochen oder fehlgeschlagen.");
        return success ? 0 : 1;
    } catch (const std::exception& error) {
        std::println(stderr, "Fehler: {}", error.what());
        return 1;
    }
}
