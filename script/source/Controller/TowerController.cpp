#include "Controller/TowerController.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Model/Tower.hpp"

using json = nlohmann::json;

static float length(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

TowerController::TowerController()
    : m_ghostVisible(false)
    , m_coins(150) // starting gold: enough for 3 basic towers
{}
//loadFromJson
bool TowerController::loadFromJson(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[TowerController] Cannot open JSON: " << path << "\n";
        return false;
    }

    json data;
    file >> data;

    try {
        auto loadTower = [&](const std::string& name,
                             const json& j,
                             const std::string& level) {
            TowerDef def;
            def.id    = j["id"].get<int>();
            def.cost  = j["cost"].get<int>();
            def.level = level;

            auto& ad = j["attackdatas"];
            def.attack.damage          = ad["damage"].get<int>();
            def.attack.range           = ad["range"].get<float>();
            def.attack.fireRate        = ad["firerate"].get<float>();
            def.attack.damagePerSecond = ad["damagepersecond"].get<float>();
            def.attack.slowness        = ad["slowness"].get<float>();
            def.attack.effectDuration  = ad["effectduration"].get<float>();
            def.attack.aoeRadius       = ad["aoeradius"].get<float>();
            def.attack.nbTarget        = ad["nbtarget"].get<int>();

            def.buildingPath   = j["paths"]["building"].get<std::string>();
            def.projectilePath = j["paths"]["projectile"].get<std::string>();
            def.soundPath      = j["paths"]["sound"].get<std::string>();

            if (!def.buildingTex.loadFromFile(
                    "../assets/" + def.buildingPath.substr(3)))
                std::cerr << "[TC] Missing building: " << def.buildingPath << "\n";

            if (!def.projectileTex.loadFromFile(
                    "../assets/" + def.projectilePath.substr(3)))
                std::cerr << "[TC] Missing projectile: " << def.projectilePath << "\n";

            m_defs[name] = std::move(def);
        };

        loadTower("basic",  data["lvl1"]["basic"],  "lvl1");
        loadTower("fire",   data["lvl2"]["fire"],   "lvl2");
        loadTower("ice",    data["lvl2"]["ice"],    "lvl2");
        loadTower("rock",   data["lvl2"]["rock"],   "lvl2");
        loadTower("arcane", data["lvl2"]["arcane"], "lvl2");
    }
    catch (std::exception& e) {
        std::cerr << "[TowerController] JSON parse error: " << e.what() << "\n";
        return false;
    }

    return true;
}
//prix de la tour
int TowerController::getCostOf(const std::string& type) const {
    auto it = m_defs.find(type);
    if (it == m_defs.end()) return 9999;
    return it->second.cost;
}

void TowerController::spawnGhost(const std::string& type) {
    auto& def = m_defs.at(type);
    m_ghost.emplace(def.buildingTex);
    m_ghost->setColor(sf::Color(255, 255, 255, 150));
    m_ghost->setOrigin({
        def.buildingTex.getSize().x / 2.f,
        def.buildingTex.getSize().y / 2.f
    });
    m_ghost->setScale({2.f, 2.f});
    m_ghostVisible = true;
}

bool TowerController::selectTower(const std::string& type) {
    m_upgradeTargetIndex = -1;
    if (m_defs.find(type) == m_defs.end()) return false;
    int cost = m_defs.at(type).cost;
    if (m_coins < cost) return false;
    m_selectedType = type;
    spawnGhost(type);
    return true;
}
//Selection d'une tour pour upgrade
void TowerController::clearSelection() {
    m_selectedType.clear();
    m_ghost.reset();
    m_ghostVisible       = false;
    m_upgradeTargetIndex = -1;
}

void TowerController::setGhostPosition(sf::Vector2f pos) {
    if (m_ghostVisible && m_ghost.has_value())
        m_ghost->setPosition(pos);
}
//placeTower
void TowerController::placeTower(sf::Vector2f pos) {
    if (m_selectedType.empty()) return;
    auto& def = m_defs[m_selectedType];
    m_coins -= def.cost;

    m_towers.emplace_back(def.buildingTex, def.projectileTex, def.attack, def.soundPath, pos, def.cost);

    clearSelection();
}

int TowerController::getTowerIndexAt(sf::Vector2f pos) const {
    constexpr float CLICK_RADIUS = 32.f;
    for (int i = 0; i < (int)m_towers.size(); ++i) {
        sf::Vector2f d = m_towers[i].getPosition() - pos;
        if (std::sqrt(d.x*d.x + d.y*d.y) <= CLICK_RADIUS)
            return i;
    }
    return -1;
}

void TowerController::selectTowerForUpgrade(int index) {
    clearSelection();
    if (index >= 0 && index < (int)m_towers.size())
        m_upgradeTargetIndex = index;
}
//upgradeTower
bool TowerController::upgradeTower(const std::string& lv2Type) {
    if (m_upgradeTargetIndex < 0 ||
        m_upgradeTargetIndex >= (int)m_towers.size())
        return false;

    auto it = m_defs.find(lv2Type);
    if (it == m_defs.end() || it->second.level != "lvl2") return false;

    int cost = it->second.cost;
    if (m_coins < cost) return false;

    m_coins -= cost;

    sf::Vector2f pos = m_towers[m_upgradeTargetIndex].getPosition();
    m_towers[m_upgradeTargetIndex] = Tower(
        it->second.buildingTex,
        it->second.projectileTex,
        it->second.attack,
        it->second.soundPath,
        pos,
        it->second.cost
    );

    m_upgradeTargetIndex = -1;
    return true;
}

bool TowerController::sellSelectedTower() {
    if (m_upgradeTargetIndex < 0 ||
        m_upgradeTargetIndex >= (int)m_towers.size())
        return false;

    int refund = int(m_defs["basic"].cost * SELL_REFUND);
    m_coins += refund;
    m_towers.erase(m_towers.begin() + m_upgradeTargetIndex);
    clearSelection();
    return true;
}
//update
void TowerController::update(float dt,
                             const std::vector<std::unique_ptr<Enemy>>& enemies) {
    for (auto& t : m_towers) t.update(dt);

    for (auto& t : m_towers) {
        if (!t.canFire()) continue;

        Enemy* best     = nullptr;
        float  bestDist = 0.f;

        for (auto& e : enemies) {
            if (e->isDead() || e->hasReached()) continue;
            float d = length(e->getPosition() - t.getPosition());
            if (d <= t.getAttack().range * 64.f) {
                if (!best || d < bestDist) {
                    best     = e.get();
                    bestDist = d;
                }
            }
        }

        if (best) {
            t.resetCooldown();
            m_projectiles.emplace_back(
                t.getProjectileTexture(),
                t.getPosition(),
                best->getPosition(),
                t.getSoundPath(),
                t.getAttack().damage
            );
        }
    }

    for (auto& p : m_projectiles) p.update(dt);

    for (auto& p : m_projectiles) {
        if (p.isDead()) continue;
        for (auto& e : enemies) {
            if (e->isDead() || e->hasReached()) continue;
            if (p.checkCollision(*e)) break;
        }
    }

    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                       [](const Projectile& p){ return p.isDead(); }),
        m_projectiles.end()
    );
}

void TowerController::render(sf::RenderWindow& window) {
    for (auto& t : m_towers)      t.render(window);
    for (auto& p : m_projectiles) p.render(window);
    if (m_ghostVisible && m_ghost.has_value())
        window.draw(*m_ghost);
}