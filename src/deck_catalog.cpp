#include "deck_catalog.hpp"

#include <array>

namespace {
constexpr auto dracotail = std::to_array<deck_recipe>({
    {"Droll & Lock Bird", "Droll.png", 2},
    {R"(Maxx "C")", "Maxx C.png", 1},
    {"Dracotail Phryxul", "Phryxul.png", 1},
    {"Ash Blossom", "Ash Blossom & Joyous Spring.png", 3},
    {"Ghost Belle", "Ghost Belle.png", 2},
    {"Mululu", "Mululu.png", 3},
    {"Fallen of Albaz", "Albaz4k.png", 1},
    {"Purulia", "Purulia.png", 2},
    {"Fuwalos", "Fuwalos.png", 3},
    {"Lukias", "Lukias.png", 3},
    {"Faimena", "Faimena.png", 3},
    {"Urgula", "Urgula.png", 2},
    {"Pan", "Pan.png", 1},
    {"Nibiru", "Nibiru.png", 2},
    {"Branded Fusion", "Branded Fusion.png", 2},
    {"Ketu Dracotail", "Ketu.png", 3},
    {"Rahu Dracotail", "Rahu.png", 3},
    {"Dracotail Horn", "Horn.png", 1},
    {"Dracotail Flame", "Flame.png", 1},
    {"Dracotail Sting", "Sting.png", 1},
    {"Gulamel", "Gulamel Deck.png", 1},
    {"Titaniklad", "Titaniklad Deck.png", 1},
    {"Albion", "Albion Deck.png", 1},
    {"Mirrorjade", "Mirrorjade Deck.png", 1},
    {"Lubellion", "Lubellion Deck.png", 1},
    {"Rindbrumm", "Rindbrumm Deck.png", 1},
    {"Sanctifire", "Sanctifire Deck.png", 1},
    {"Khaos", "Khaos Deck.png", 1},
    {"Filia Regis", "Filia Regis Deck.png", 1},
    {"Arthalion ", "Arthalion Deck.png", 3},
    {"Secreterion", "Secreterion Deck.png", 1},
    {"Verte", "Verte Deck.png", 1},
    {"Little Knight", "Little Knight Deck.png", 1}
});


constexpr auto vanquish = std::to_array<deck_recipe>({
    {R"(Maxx "C")", "K9 Vanquish Soul/Maxx C.png", 1},
    {"Ash Blossom", "K9 Vanquish Soul/Ash Blossom & Joyous Spring.png", 3},
    {"Razen", "K9 Vanquish Soul/Razen.png", 2},
    {"Mad Love", "K9 Vanquish Soul/Mad Love.png", 3},
    {"Purulia", "K9 Vanquish Soul/Purulia.png", 2},
    {"Fuwalos", "K9 Vanquish Soul/Fuwalos.png", 3},
    {"Jiaolong", "K9 Vanquish Soul/Jialong.png", 2},
    {"Izuna", "K9 Vanquish Soul/Izuna.png", 3},
    {"Jokul", "K9 Vanquish Soul/Jokul.png", 2},
    {"Lantern", "K9 Vanquish Soul/Lantern.png", 1},
    {"Lupis", "K9 Vanquish Soul/Lupis.png", 1},
    {"Hollie Sue", "K9 Vanquish Soul/Hollie Sue.png", 3},
    {"Noroi", "K9 Vanquish Soul/Noroi.png", 1},
    {"Shifter", "K9 Vanquish Soul/Shifter.png", 1},
    {"Borger", "K9 Vanquish Soul/Borger.png", 1},
    {"Valius", "K9 Vanquish Soul/Valius.png", 1},
    {"Reinforcement", "K9 Vanquish Soul/Rota.png", 1},
    {"Talent", "K9 Vanquish Soul/Talents.png", 1},
    {"Stake your Soul", "K9 Vanquish Soul/Stake.png", 2},
    {"Vanquish Soul, Start!", "K9 Vanquish Soul/Start.png", 1},
    {"Case for", "K9 Vanquish Soul/Case for.png", 3},
    {"Forced Release", "K9 Vanquish Soul/Forced Release.png", 1},
    {"Snow Devil", "K9 Vanquish Soul/Snow Devil.png", 1},
    {"104", "K9 Vanquish Soul/104.png", 1},
    {"Tiras", "K9 Vanquish Soul/Tiras.png", 1},
    {"C104", "K9 Vanquish Soul/C104.png", 1},
    {"N.As.H. Knight", "K9 Vanquish Soul/Nash Knight.png", 1},
    {"Ripper", "K9 Vanquish Soul/Ripper.png", 1},
    {"Vallon", "K9 Vanquish Soul/Vallon.png", 1},
    {"Jacks", "K9 Vanquish Soul/Jacks.png", 1},
    {"CXyz", "K9 Vanquish Soul/Xyz Nash Knight.png", 1},
    {"Werewolf", "K9 Vanquish Soul/Werewolf.png", 1},
    {"Zeus", "K9 Vanquish Soul/Zeus.png", 1},
    {"Ty-Phon", "K9 Vanquish Soul/Ty-Phon.png", 1},
    {"Rock of the Van", "K9 Vanquish Soul/Rock.png", 2},
    {"Saryuja", "K9 Vanquish Soul/Saryuja.png", 1},
    {"Little Knight", "K9 Vanquish Soul/Little Knight Deck.png", 1},

});
} // namespace

std::span<const deck_recipe> select_deck(int choice) {
    switch (choice) {
    case 1: return dracotail;
    case 2: return vanquish;
    default: return {};
    }
}
