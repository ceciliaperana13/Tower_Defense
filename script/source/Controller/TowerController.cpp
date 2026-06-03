#include "TowerController.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cmath>

using json = nlohmann::json;

static float length(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

TowerController::TowerController()
    : m_ghostSprite(m_basicTex) // SFML3 : sprite doit être construit avec texture
{
}

bool TowerController::loadFromJson(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        std::cerr << "[TowerController] Cannot open " << path << "\n";
        return false;
    }

    json j;
    ifs >> j;

    try {
        auto& basic = j["lvl1"]["basic"];
        m_basicDef.id = basic["id"].get<int>();

        auto& ad = basic["attackdatas"];
        m_basicDef.attack.damage           = ad["damage"].get<int>();
        m_basicDef.attack.range            = ad["range"].get<float>();
        m_basicDef.attack.fireRate         = ad["firerate"].get<float>();
        m_basicDef.attack.damagePerSecond  = ad["damagepersecond"].get<float>();
        m_basicDef.attack.slowness         = ad["slowness"].get<float>();
        m_basicDef.attack.effectDuration   = ad["effectduration"].get<float>();
        m_basicDef.attack.aoeRadius        = ad["aoeradius"].get<float>();
        m_basicDef.attack.nbTarget         = ad["nbtarget"].get<int>();

        auto& paths = basic["paths"];
        m_basicDef.buildingPath   = paths["building"].get<std::string>();
        m_basicDef.projectilePath = paths["projectile"].get<std::string>();
    }
    catch (std::exception& e) {
        std::cerr << "[TowerController] JSON parse error: " << e.what() << "\n";
        return false;
    }

    // Charger sprite tour
    if (!m_basicTex.loadFromFile("../assets/" + m_basicDef.buildingPath.substr(3))) {
        std::cerr << "[TowerController] Missing building texture: "
                  << m_basicDef.buildingPath << "\n";
    }

    // Charger sprite projectile
    if (!m_basicProjectileTex.loadFromFile("../assets/" + m_basicDef.projectilePath.substr(3))) {
        std::cerr << "[TowerController] Missing projectile texture: "
                  << m_basicDef.projectilePath << "\n";
    }

    // Ghost
    m_ghostSprite = sf::Sprite(m_basicTex);
    m_ghostSprite.setColor(sf::Color(255, 255, 255, 150));
    m_ghostSprite.setOrigin(sf::Vector2f(
        m_basicTex.getSize().x / 2.f,
        m_basicTex.getSize().y / 2.f
    ));
    m_ghostSprite.setScale(sf::Vector2f(2.f, 2.f));

    return true;
}

void TowerController::selectBasic() {
    m_hasSelection = true;
    m_ghostVisible = true;
    m_ghostSprite.setTexture(m_basicTex);
}

void TowerController::clearSelection() {
    m_hasSelection = false;
    m_ghostVisible = false;
}

void TowerController::setGhostPosition(sf::Vector2f worldPos) {
    if (!m_hasSelection) return;
    m_ghostVisible = true;
    m_ghostSprite.setPosition(worldPos);
}

void TowerController::placeCurrentTower(sf::Vector2f worldPos) {
    if (!m_hasSelection) return;

    Tower t(m_basicTex);
    t.position     = worldPos;
    t.attack       = m_basicDef.attack;
    t.fireCooldown = 0.f;

    t.sprite.setOrigin(sf::Vector2f(
        m_basicTex.getSize().x / 2.f,
        m_basicTex.getSize().y / 2.f
    ));
    t.sprite.setScale(sf::Vector2f(2.f, 2.f));
    t.sprite.setPosition(worldPos);

    m_towers.push_back(t);
}

Enemy* TowerController::findTarget(const Tower& tower,
                                   const std::vector<std::unique_ptr<Enemy>>& enemies) {
    Enemy* best = nullptr;
    float bestDist = 0.f;

    for (auto& e : enemies) {
        if (e->isDead() || e->hasReached()) continue;

        sf::Vector2f enemyPos = e->getPosition();
        float d = length(enemyPos - tower.position);

        if (d <= tower.attack.range * 64.f) {
            if (!best || d < bestDist) {
                best = e.get();
                bestDist = d;
            }
        }
    }
    return best;
}

void TowerController::fireFromTower(Tower& tower,
                                    const std::vector<std::unique_ptr<Enemy>>& enemies,
                                    float dt) {
    tower.fireCooldown -= dt;
    if (tower.fireCooldown > 0.f) return;

    Enemy* target = findTarget(tower, enemies);
    if (!target) return;

    tower.fireCooldown = 1.f / tower.attack.fireRate;

    Projectile p(m_basicProjectileTex);
    p.sprite.setOrigin(sf::Vector2f(
        m_basicProjectileTex.getSize().x / 2.f,
        m_basicProjectileTex.getSize().y / 2.f
    ));
    p.sprite.setScale(sf::Vector2f(2.f, 2.f));
    p.sprite.setPosition(tower.position);
    p.damage = tower.attack.damage;

    sf::Vector2f dir = target->getPosition() - tower.position;
    float len = length(dir);
    if (len == 0.f) return;
    dir /= len;

    p.velocity = dir * 250.f;
    p.lifeTime = 2.f;

    m_projectiles.push_back(p);
}

void TowerController::updateProjectiles(float dt,
                                        const std::vector<std::unique_ptr<Enemy>>& enemies) {
    for (auto& p : m_projectiles) {
        p.lifeTime -= dt;
        p.sprite.move(p.velocity * dt);

        for (auto& e : enemies) {
            if (e->isDead() || e->hasReached()) continue;

            auto inter = p.sprite.getGlobalBounds().findIntersection(e->getBounds());
            if (inter.has_value()) {
                e->takeDamage(p.damage);
                p.lifeTime = 0.f;
                break;
            }
        }
    }

    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                       [](const Projectile& p) { return p.lifeTime <= 0.f; }),
        m_projectiles.end()
    );
}

void TowerController::update(float dt,
                             const std::vector<std::unique_ptr<Enemy>>& enemies) {
    for (auto& t : m_towers)
        fireFromTower(t, enemies, dt);

    updateProjectiles(dt, enemies);
}

void TowerController::render(sf::RenderWindow& window) {
    for (auto& t : m_towers)
        window.draw(t.sprite);

    for (auto& p : m_projectiles)
        window.draw(p.sprite);

    if (m_ghostVisible && m_hasSelection)
        window.draw(m_ghostSprite);
}
