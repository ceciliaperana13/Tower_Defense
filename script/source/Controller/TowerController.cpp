#include "TowerController.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cmath>

using json = nlohmann::json;

static float length(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

TowerController::TowerController() {
    m_ghostVisible = false;
}

bool TowerController::loadFromJson(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[TowerController] Cannot open JSON: " << path << "\n";
        return false;
    }

    json data;
    file >> data;

    try {
        auto& lvl1 = data["lvl1"];
        auto& lvl2 = data["lvl2"];

        auto loadTower = [&](const std::string& name, const json& j) {
            TowerDef def;
            def.id = j["id"].get<int>();

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

            if (!def.buildingTex.loadFromFile("../assets/" + def.buildingPath.substr(3)))
                std::cerr << "[TowerController] Missing building sprite: " << def.buildingPath << "\n";

            if (!def.projectileTex.loadFromFile("../assets/" + def.projectilePath.substr(3)))
                std::cerr << "[TowerController] Missing projectile sprite: " << def.projectilePath << "\n";

            m_defs[name] = def;
        };

        loadTower("basic",  lvl1["basic"]);
        loadTower("fire",   lvl2["fire"]);
        loadTower("ice",    lvl2["ice"]);
        loadTower("rock",   lvl2["rock"]);
        loadTower("arcane", lvl2["arcane"]);
    }
    catch (std::exception& e) {
        std::cerr << "[TowerController] JSON parse error: " << e.what() << "\n";
        return false;
    }

    return true;
}

void TowerController::selectTower(const std::string& type) {
    if (m_defs.find(type) == m_defs.end()) {
        std::cerr << "[TowerController] Unknown tower type: " << type << "\n";
        return;
    }

    m_selectedType = type;
    m_ghostVisible = true;

    auto& def = m_defs[type];

    m_ghost.emplace(def.buildingTex);
    m_ghost->setColor(sf::Color(255, 255, 255, 150));
    m_ghost->setOrigin(sf::Vector2f(
        def.buildingTex.getSize().x / 2.f,
        def.buildingTex.getSize().y / 2.f
    ));
    m_ghost->setScale(sf::Vector2f(2.f, 2.f));
}

void TowerController::setGhostPosition(sf::Vector2f pos) {
    if (!m_ghostVisible || !m_ghost.has_value()) return;
    m_ghost->setPosition(pos);
}

void TowerController::placeTower(sf::Vector2f pos) {
    if (m_selectedType.empty()) return;

    auto& def = m_defs[m_selectedType];

    m_towers.emplace_back(
        def.buildingTex,
        def.projectileTex,
        def.attack,
        pos
    );

    m_ghostVisible = false;
    m_ghost.reset();
    m_selectedType.clear();
}

void TowerController::update(float dt,
                             const std::vector<std::unique_ptr<Enemy>>& enemies) {

    for (auto& t : m_towers)
        t.update(dt);

    for (auto& t : m_towers) {
        if (!t.canFire()) continue;

        Enemy* best = nullptr;
        float bestDist = 0.f;

        for (auto& e : enemies) {
            if (e->isDead() || e->hasReached()) continue;

            float d = length(e->getPosition() - t.getPosition());
            if (d <= t.getAttack().range * 64.f) {
                if (!best || d < bestDist) {
                    best = e.get();
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
                t.getAttack().damage
            );
        }
    }

    for (auto& p : m_projectiles)
        p.update(dt);

    for (auto& p : m_projectiles) {
        if (p.isDead()) continue;

        for (auto& e : enemies) {
            if (e->isDead() || e->hasReached()) continue;

            if (p.checkCollision(*e))
                break;
        }
    }

    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                       [](const Projectile& p){ return p.isDead(); }),
        m_projectiles.end()
    );
}

void TowerController::render(sf::RenderWindow& window) {
    for (auto& t : m_towers)
        t.render(window);

    for (auto& p : m_projectiles)
        p.render(window);

    if (m_ghostVisible && m_ghost.has_value())
        window.draw(*m_ghost);
}
