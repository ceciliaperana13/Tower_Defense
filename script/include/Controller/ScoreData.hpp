#pragma once
#include <string>

struct ScoreData {
    std::string playerName;
    int         score { 0 };
    int         wave  { 0 };
    std::string date;
};