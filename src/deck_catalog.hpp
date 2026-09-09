#pragma once

#include <cstdint>
#include <span>
#include <string_view>

struct deck_recipe {
    std::string_view name;
    std::string_view deck_path;
    std::uint8_t amount;
};

std::span<const deck_recipe> select_deck(int choice);
