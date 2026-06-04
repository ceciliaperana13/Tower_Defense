#pragma once
#include <string>

struct ScoreData {
    std::string playerName  = "Player";
    int         score       = 0;
    int         enemyCount  = 0;   // ennemis tués
    int         wave        = 0;   // vague atteinte
    std::string date        = "";
};