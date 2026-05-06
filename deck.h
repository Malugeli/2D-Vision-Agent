#pragma once
#include <vector>
#include <array>
#include <string>

struct deck_recipe{
    std::string_view name;
    std::string_view deck_path;
    uint8_t amount;
};

constexpr auto dracotail = std::to_array<deck_recipe>({
    {"Droll & Lock Bird", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Droll.png", 2},
    {R"(Maxx "C")", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Maxx C.png", 1},
    {"Dracotail Phryxul", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Phryxul.png", 1},
    {"Ash Blossom", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Ash Blossom & Joyous Spring.png", 3},
    {"Ghost Belle", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Ghost Belle.png", 2},
    {"Mululu", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Mululu.png", 3},
    {"Fallen of Albaz", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Albaz4k.png", 1},
    {"Purulia", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Purulia.png", 2},
    {"Fuwalos", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Fuwalos.png", 3},
    {"Lukias", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Lukias.png", 3},
    {"Faimena", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Faimena.png", 3},
    {"Urgula", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Urgula.png", 2},
    {"Pan", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Pan.png", 1},
    {"Nibiru", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Nibiru.png", 2},
    {"Branded Fusion", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Branded Fusion.png", 2},
    {"Ketu Dracotail", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Ketu.png", 3},
    {"Rahu Dracotail", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Rahu.png", 3},
    {"Dracotail Horn", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Horn.png", 1},
    {"Dracotail Flame", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Flame.png", 1},
    {"Dracotail Sting", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Sting.png", 1},
    {"Gulamel", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Gulamel Deck.png", 1},
    {"Titaniklad", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Titaniklad Deck.png", 1},
    {"Albion", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Albion Deck.png", 1},
    {"Mirrorjade", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Mirrorjade Deck.png", 1},
    {"Lubellion", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Lubellion Deck.png", 1},
    {"Rindbrumm", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Rindbrumm Deck.png", 1},
    {"Sanctifire", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Sanctifire Deck.png", 1},
    {"Khaos", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Khaos Deck.png", 1},
    {"Filia Regis", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Filia Regis Deck.png", 1},
    {"Arthalion ", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Arthalion Deck.png", 3},
    {"Secreterion", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Secreterion Deck.png", 1},
    {"Verte", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Verte Deck.png", 1},
    {"Little Knight", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/Little Knight Deck.png", 1}
});


constexpr auto vanquish = std::to_array<deck_recipe>({
    {R"(Maxx "C")", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Maxx C.png", 1},
    {"Ash Blossom", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Ash Blossom & Joyous Spring.png", 3},
    {"Razen", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Razen.png", 2},
    {"Mad Love", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Mad Love.png", 3},
    {"Purulia", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Purulia.png", 2},
    {"Fuwalos", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Fuwalos.png", 3},
    {"Jiaolong", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Jialong.png", 2},
    {"Izuna", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Izuna.png", 3},
    {"Jokul", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Jokul.png", 2},
    {"Lantern", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Lantern.png", 1},
    {"Lupis", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Lupis.png", 1},
    {"Hollie Sue", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Hollie Sue.png", 3},
    {"Noroi", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Noroi.png", 1},
    {"Shifter", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Shifter.png", 1},
    {"Borger", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Borger.png", 1},
    {"Valius", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Valius.png", 1},
    {"Reinforcement", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Rota.png", 1},
    {"Talent", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Talents.png", 1},
    {"Stake your Soul", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Stake.png", 2},
    {"Vanquish Soul, Start!", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Start.png", 1},
    {"Case for", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Case for.png", 3},
    {"Forced Release", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Forced Release.png", 1},
    {"Snow Devil", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Snow Devil.png", 1},
    {"104", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/104.png", 1},
    {"Tiras", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Tiras.png", 1},
    {"C104", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/C104.png", 1},
    {"N.As.H. Knight", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Nash Knight.png", 1},
    {"Ripper", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Ripper.png", 1},
    {"Vallon", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Vallon.png", 1},
    {"Jacks", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Jacks.png", 1},
    {"CXyz", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Xyz Nash Knight.png", 1},
    {"Werewolf", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Werewolf.png", 1},
    {"Zeus", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Zeus.png", 1},
    {"Ty-Phon", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Ty-Phon.png", 1},
    {"Rock of the Van", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Rock.png", 2},
    {"Saryuja", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Saryuja.png", 1},
    {"Little Knight", "C:/Users/aluge/Desktop/Computer Science/Projekte/YgoBotMaher/Pics/K9 Vanquish Soul/Little Knight Deck.png", 1},

});