#pragma once
#include <array>
#include <utility>
#include <windows.h>
#include <cmath>


struct Faktorisierung{
    double x;
    double y;
};

enum class UiTarget{
    out,
    in,
    scrollbar,
    searchbar,

    COUNT // Sagt die Menge der Elemente an
    };

constexpr std::array<Faktorisierung, std::to_underlying(UiTarget::COUNT)> UI{{

    {0.828515625, 0.4701388888888889}, //Das ist für "OUT"
    {0.4609375, 0.3951388888888889}, //Das ist für "IN"
    {0.9703125, 0.3548611111111111}, // Scrollbar
    {0.781640625, 0.1909722222222222} // Searchbar

}};

inline POINT get_cords(UiTarget target, long width, long height){
    return {std::lround(width * UI[std::to_underlying(target)].x),
    std::lround(height * UI[std::to_underlying(target)].y)};
}