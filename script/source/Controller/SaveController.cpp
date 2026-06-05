#include "Controller/SaveController.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdio>   // std::rename, std::remove

// ─── Constructeur
SaveController::SaveController(const std::string& scoresPath,
                               const std::string& towerValPath,
                               const std::string& enemyValPath)
    : m_scoresPath(scoresPath)
    , m_towerValPath(towerValPath)
    , m_enemyValPath(enemyValPath)
{}

// ─── readFile
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

// ─── writeFileAtomic  (static → appelable depuis const et non-const)
// Sur Windows, rename() échoue si la destination existe → on supprime d'abord
void SaveController::writeFileAtomic(const std::string& path,
                                     const std::string& content) {
    const std::string tmp = path + ".tmp";

    // 1. Écriture dans le fichier temporaire
    {
        std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
        if (!f.is_open()) {
            std::cerr << "[SaveController] Impossible d'écrire : " << tmp << "\n";
            // Fallback : écriture directe
            std::ofstream fallback(path, std::ios::binary | std::ios::trunc);
            if (fallback.is_open()) fallback << content;
            return;
        }
        f << content;
        f.flush();
    } // fermeture du fichier ici avant rename

    // 2. Sur Windows : supprimer la destination avant rename
    std::remove(path.c_str());

    // 3. Renommage atomique
    if (std::rename(tmp.c_str(), path.c_str()) != 0) {
        std::cerr << "[SaveController] rename() echoue pour " << path
                  << " — écriture directe en fallback\n";
        // Fallback : copie directe si rename échoue encore
        std::ofstream fallback(path, std::ios::binary | std::ios::trunc);
        if (fallback.is_open()) {
            fallback << content;
            fallback.flush();
        }
        std::remove(tmp.c_str());
    }
}

// ─── scoreToJson
std::string SaveController::scoreToJson(const ScoreData& s) {
    return "  {\n"
           "    \"playerName\": \"" + s.playerName                 + "\",\n"
           "    \"score\": "        + std::to_string(s.score)      + ",\n"
           "    \"enemyCount\": "   + std::to_string(s.enemyCount) + ",\n"
           "    \"wave\": "         + std::to_string(s.wave)       + ",\n"
           "    \"date\": \""       + s.date                       + "\"\n"
           "  }";
}

// ─── extractField (helper local)
static std::string extractField(const std::string& block, const std::string& key) {
    const std::string search = "\"" + key + "\"";
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
    }
    auto end = block.find_first_of(",\n}", pos);
    std::string val = block.substr(pos, end - pos);
    val.erase(std::remove(val.begin(), val.end(), ' '), val.end());
    return val;
}

// ─── jsonToScore
ScoreData SaveController::jsonToScore(const std::string& block) {
    ScoreData s;
    s.playerName = extractField(block, "playerName");
    s.date       = extractField(block, "date");
    try { s.score      = std::stoi(extractField(block, "score"));      } catch (...) {}
    try { s.enemyCount = std::stoi(extractField(block, "enemyCount")); } catch (...) {}
    try { s.wave       = std::stoi(extractField(block, "wave"));       } catch (...) {}
    return s;
}

// ─── splitJsonArray
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
    auto scores = loadScores();

    ScoreData entry = s;
    if (entry.date.empty()) {
        std::time_t t = std::time(nullptr);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", std::localtime(&t));
        entry.date = buf;
    }
    scores.push_back(entry);

    std::sort(scores.begin(), scores.end(),
        [](const ScoreData& a, const ScoreData& b){ return a.score > b.score; });
    if (scores.size() > 10) scores.resize(10);

    std::string json = "[\n";
    for (std::size_t i = 0; i < scores.size(); ++i) {
        json += scoreToJson(scores[i]);
        if (i + 1 < scores.size()) json += ",";
        json += "\n";
    }
    json += "]\n";

    writeFileAtomic(m_scoresPath, json);

    std::cout << "[SaveController] Score sauvegarde : " << entry.playerName
              << "  score=" << entry.score
              << "  kills=" << entry.enemyCount
              << "  wave="  << entry.wave << "\n";
}

// ─── loadScores
std::vector<ScoreData> SaveController::loadScores() const {
    const std::string json = readFile(m_scoresPath);
    if (json.empty()) return {};
    const auto blocks = splitJsonArray(json);
    std::vector<ScoreData> scores;
    scores.reserve(blocks.size());
    for (const auto& b : blocks)
        scores.push_back(jsonToScore(b));
    return scores;
}

// ─── loadTowerValues / loadEnemyValues
std::string SaveController::loadTowerValues() const { return readFile(m_towerValPath); }
std::string SaveController::loadEnemyValues() const { return readFile(m_enemyValPath); }

// ─── exportSave  (const → OK car writeFileAtomic est static)
void SaveController::exportSave(const std::string& outPath) const {
    const std::string scores  = readFile(m_scoresPath);
    const std::string towers  = readFile(m_towerValPath);
    const std::string enemies = readFile(m_enemyValPath);
    const std::string out =
        "{\n"
        "  \"scores\": "  + (scores.empty()  ? "[]" : scores)  + ",\n"
        "  \"towers\": "  + (towers.empty()  ? "{}" : towers)  + ",\n"
        "  \"enemies\": " + (enemies.empty() ? "{}" : enemies) + "\n"
        "}\n";
    writeFileAtomic(outPath, out);
    std::cout << "[SaveController] Export -> " << outPath << "\n";
}