#include "deck_selection.hpp"

std::optional<int> parse_deck_choice(std::string_view text) {
    if (text == "1") return 1;
    if (text == "2") return 2;
    return std::nullopt;
}
