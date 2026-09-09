#include "deck_catalog.hpp"
#include "deck_selection.hpp"
#include <filesystem>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;
    int failures = 0;
    const auto check = [&](bool condition) { if (!condition) ++failures; };
    check(parse_deck_choice("1") == 1);
    check(parse_deck_choice("2") == 2);
    for (const auto invalid : {"", "0", "3", "-1", "1abc", "2.0", "99999999999999999999"})
        check(!parse_deck_choice(invalid));
    check(select_deck(0).empty());
    check(select_deck(3).empty());
    for (int choice : {1, 2}) {
        const auto deck = select_deck(choice);
        check(!deck.empty());
        for (const auto& card : deck) {
            check(!card.name.empty());
            check(card.amount > 0 && card.amount <= 3);
            const auto path = std::filesystem::path(argv[1]) / card.deck_path;
            if (!std::filesystem::is_regular_file(path)) {
                std::cerr << "Fehlendes Bild: " << path << '\n';
                ++failures;
            }
        }
    }
    return failures == 0 ? 0 : 1;
}
