#pragma once
#include <vector>
#include <array>
#include <string>

struct deck_recipe{
    std::string_view name;
    std::string_view deck_path;
    std::string_view editor_path;
    uint8_t amount;
};

auto dracotail = std::to_array<deck_recipe>({
    {"Droll & Lock Bird", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Droll.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Droll klein.png", 2},
    {R"(Maxx "C")", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Maxx C.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Maxx C klein.png", 1},
    {"Dracotail Phryxul", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Phryxul.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Phryxul klein.png", 1},
    {"Ash Blossom", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Ash Blossom & Joyous Spring.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Ash Blossom & Joyous Spring klein.png", 3},
    {"Ghost Belle", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Ghost Belle.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Ghost Belle klein.png", 2},
    {"Mululu", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Mululu.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Mululu klein.png", 3},
    {"Fallen of Albaz", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Albaz4k.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Albaz4k klein.png", 1},
    {"Purulia", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Purulia.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Purulia klein.png", 2},
    {"Fuwalos", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Fuwalos.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Fuwalos klein.png", 3},
    {"Lukias", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Lukias.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Lukias klein.png", 3},
    {"Faimena", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Faimena.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Faimena klein.png", 3},
    {"Urgula", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Urgula.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Urgula klein.png", 2},
    {"Pan", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Pan.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Pan klein.png", 1},
    {"Nibiru", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Nibiru.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Nibiru klein.png", 2},
    {"Branded Fusion", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Branded Fusion.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Branded Fusion klein.png", 2},
    {"Ketu Dracotail", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Ketu.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Ketu klein.png", 3},
    {"Rahu Dracotail", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Rahu.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Rahu klein.png", 3},
    {"Dracotail Horn", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Horn.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Horn klein.png", 1},
    {"Dracotail Flame", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Flame.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Flame klein.png", 1},
    {"Dracotail Sting", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Sting.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Sting klein.png", 1},
    {"Gulamel", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Gulamel Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Gulamel editor.png", 1},
    {"Titaniklad", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Titaniklad Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Titaniklad editor.png", 1},
    {"Albion", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Albion Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Albion editor.png", 1},
    {"Mirrorjade", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Mirrorjade Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Mirrorjade editor.png", 1},
    {"Lubellion", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Lubellion Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Lubellion editor.png", 1},
    {"Rindbrumm", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Rindbrumm Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Rindbrumm editor.png", 1},
    {"Sanctifire", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Sanctifire Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Sanctifire editor.png", 1},
    {"Khaos", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Khaos Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Khaos editor.png", 1},
    {"Filia Regis", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Filia Regis Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Filia Regis editor.png", 1},
    {"Arthalion ", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Arthalion Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Arthalion editor.png", 3},
    {"Secreterion", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Secreterion Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Secreterion editor.png", 1},
    {"Verte", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Verte Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Verte editor.png", 1},
    {"Little Knight", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Little Knight Deck.png", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Little Knight editor.png", 1}
});