#pragma once

#include "ScoreData.hpp"
#include <vector>
#include <string>

class SaveController {
public:
    explicit SaveController(
    const std::string& scoresPath   = "../assets/data/scores.json",
    const std::string& towerValPath = "../assets/data/tower_values.json",
    const std::string& enemyValPath = "../assets/data/enemy_values.json"
);

    // Scores
    void                    saveScore(const ScoreData& s);
    std::vector<ScoreData>  loadScores() const;

    // Config JSON brut (towers / enemies)
    std::string loadTowerValues() const;
    std::string loadEnemyValues() const;

    // Export tout en un fichier
    void exportSave(const std::string& outPath = "../assets/data/export.json") const;

private:
    std::string m_scoresPath;
    std::string m_towerValPath;
    std::string m_enemyValPath;

    // Helpers JSON minimaliste (sans dépendance externe)
    static std::string        readFile (const std::string& path);
    static void               writeFile(const std::string& path, const std::string& content);
    static std::string        scoreToJson  (const ScoreData& s);
    static ScoreData          jsonToScore  (const std::string& block);
    static std::vector<std::string> splitJsonArray(const std::string& json);
};