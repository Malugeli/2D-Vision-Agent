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
    deck_Begin,
    deck_End,
    editor_Begin,
    editor_End,
    deckname,
    savedeck,

    COUNT // Sagt die Menge der Elemente an
    };

constexpr std::array<Faktorisierung, std::to_underlying(UiTarget::COUNT)> UI{{

    {0.828515625, 0.4701388888888889}, //Das ist für "OUT"
    {0.4609375, 0.3951388888888889}, //Das ist für "IN"
    {0.9703125, 0.3548611111111111}, // Scrollbar
    {0.781640625, 0.1909722222222222}, // Searchbar
    {0.26, 0.19},                       // DeckBegin
    {0.67, 0.94},                       // DeckEnd
    {0.68, 0.28},                       // EditorBegin
    {0.97, 0.94},                       // EditorEnd
    {0.4, 0.12},                       // Deckname
    {0.88, 0.03}                        // Savedeck

    
}};

// Warte, wenn ClientSide immer gleich ist, warum schreibe ich nur die Faktorisierung und Clientside? Warum Hardcode ich nicht schon vorher die Clientsidepixel und das einzige 
// was ich dann in meinem Programn tun muss ist die ClientToScreen
// Sprich ich hab sagen wir 3 Tabellen, jeweils für 4k, 2k und full hd. Alles schon berechnet und es gibt 100% ein if irgendwas dann lade diese 