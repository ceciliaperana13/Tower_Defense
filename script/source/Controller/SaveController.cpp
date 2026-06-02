#include "SaveController.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <ctime>

// ─── Constructeur
SaveController::SaveController(const std::string& scoresPath,
                               const std::string& towerValPath,
                               const std::string& enemyValPath)
    : m_scoresPath(scoresPath)
    , m_towerValPath(towerValPath)
    , m_enemyValPath(enemyValPath)
{}

// ─── File I/O 
std::string SaveController::readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "[SaveController] Fichier introuvable : " << path << "\n";
        return "";
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

void SaveController::writeFile(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    if (!f.is_open()) {
        std::cerr << "[SaveController] Impossible d'ecrire : " << path << "\n";
        return;
    }
    f << content;
}

// ─── JSON helpers 
std::string SaveController::scoreToJson(const ScoreData& s) {
    return "  {\n"
           "    \"playerName\": \"" + s.playerName + "\",\n"
           "    \"score\": "        + std::to_string(s.score) + ",\n"
           "    \"wave\": "         + std::to_string(s.wave)  + ",\n"
           "    \"date\": \""       + s.date + "\"\n"
           "  }";
}

// Extrait la valeur d'une clé string ou int dans un bloc JSON minimal
static std::string extractField(const std::string& block, const std::string& key) {
    std::string search = "\"" + key + "\"";
    auto pos = block.find(search);
    if (pos == std::string::npos) return "";
    pos = block.find(':', pos);
    if (pos == std::string::npos) return "";
    ++pos;
    while (pos < block.size() && (block[pos] == ' ' || block[pos] == '\n')) ++pos;

    if (block[pos] == '"') {
        ++pos;
        auto end = block.find('"', pos);
        return block.substr(pos, end - pos);
    } else {
        auto end = block.find_first_of(",\n}", pos);
        std::string val = block.substr(pos, end - pos);
        val.erase(std::remove(val.begin(), val.end(), ' '), val.end());
        return val;
    }
}

ScoreData SaveController::jsonToScore(const std::string& block) {
    ScoreData s;
    s.playerName = extractField(block, "playerName");
    s.date       = extractField(block, "date");
    try { s.score = std::stoi(extractField(block, "score")); } catch (...) {}
    try { s.wave  = std::stoi(extractField(block, "wave"));  } catch (...) {}
    return s;
}

std::vector<std::string> SaveController::splitJsonArray(const std::string& json) {
    std::vector<std::string> blocks;
    int depth = 0;
    std::size_t start = std::string::npos;

    for (std::size_t i = 0; i < json.size(); ++i) {
        if (json[i] == '{') {
            if (depth == 0) start = i;
            ++depth;
        } else if (json[i] == '}') {
            --depth;
            if (depth == 0 && start != std::string::npos) {
                blocks.push_back(json.substr(start, i - start + 1));
                start = std::string::npos;
            }
        }
    }
    return blocks;
}

// ─── saveScore 
void SaveController::saveScore(const ScoreData& s) {
    // Charge les scores existants
    auto scores = loadScores();

    // Ajoute le nouveau
    ScoreData entry = s;
    if (entry.date.empty()) {
        std::time_t t = std::time(nullptr);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", std::localtime(&t));
        entry.date = buf;
    }
    scores.push_back(entry);

    // Trie par score décroissant, garde les 10 meilleurs
    std::sort(scores.begin(), scores.end(),
        [](const ScoreData& a, const ScoreData& b){ return a.score > b.score; });
    if (scores.size() > 10) scores.resize(10);

    // Sérialise
    std::string json = "[\n";
    for (std::size_t i = 0; i < scores.size(); ++i) {
        json += scoreToJson(scores[i]);
        if (i + 1 < scores.size()) json += ",";
        json += "\n";
    }
    json += "]\n";

    writeFile(m_scoresPath, json);
    std::cout << "[SaveController] Score sauvegarde pour " << entry.playerName << "\n";
}

// ─── loadScores 
std::vector<ScoreData> SaveController::loadScores() const {
    std::string json = readFile(m_scoresPath);
    if (json.empty()) return {};

    auto blocks = splitJsonArray(json);
    std::vector<ScoreData> scores;
    scores.reserve(blocks.size());
    for (const auto& b : blocks)
        scores.push_back(jsonToScore(b));
    return scores;
}

// ─── loadTowerValues / loadEnemyValues ───────────────────────────
std::string SaveController::loadTowerValues() const {
    return readFile(m_towerValPath);
}

std::string SaveController::loadEnemyValues() const {
    return readFile(m_enemyValPath);
}

// ─── exportSave 
void SaveController::exportSave(const std::string& outPath) const {
    std::string scores = readFile(m_scoresPath);
    std::string towers = readFile(m_towerValPath);
    std::string enemies= readFile(m_enemyValPath);

    std::string out = "{\n"
        "  \"scores\": "  + (scores.empty()  ? "[]" : scores)  + ",\n"
        "  \"towers\": "  + (towers.empty()  ? "{}" : towers)  + ",\n"
        "  \"enemies\": " + (enemies.empty() ? "{}" : enemies) + "\n"
        "}\n";

    writeFile(outPath, out);
    std::cout << "[SaveController] Export -> " << outPath << "\n";
}