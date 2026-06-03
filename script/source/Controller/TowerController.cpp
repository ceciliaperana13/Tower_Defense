#include "TowerController.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// ─────────────────────────────────────────────
// loadFromJson
// ─────────────────────────────────────────────
bool TowerController::loadFromJson(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        std::cerr << "[TowerController] Cannot open: " << jsonPath << "\n";
        return false;
    }

    json root;
    try {
        file >> root;
    } catch (const json::parse_error& e) {
        std::cerr << "[TowerController] JSON parse error: " << e.what() << "\n";
        return false;
    }

    for (auto& [levelKey, levelNode] : root.items()) {
        if (!levelNode.is_object()) continue;

        for (auto& [towerName, towerNode] : levelNode.items()) {
            if (towerNode.contains("attackdatas")) {
                parseTowerEntry(towerName, towerNode);
            }
            else if (towerNode.is_object()) {
                for (auto& [subName, subNode] : towerNode.items()) {
                    if (subNode.contains("attackdatas"))
                        parseTowerEntry(subName, subNode);
                }
            }
        }
    }

    std::cout << "[TowerController] Loaded " << towerDefs_.size() << " tower(s).\n";
    return true;
}

// ─────────────────────────────────────────────
// parseTowerEntry
// ─────────────────────────────────────────────
void TowerController::parseTowerEntry(const std::string& name, const json& node) {
    TowerData data;
    data.name = name;
    data.id   = node.value("id", 0);

    const auto& ad = node["attackdatas"];
    data.attackData.damage          = ad.value("damage",          0.f);
    data.attackData.range           = ad.value("range",           0.f);
    data.attackData.fireRate        = ad.value("firerate",        0.f);
    data.attackData.damagePerSecond = ad.value("damagepersecond", 0.f);
    data.attackData.slowness        = ad.value("slowness",        0.f);
    data.attackData.effectDuration  = ad.value("effectduration",  0.f);
    data.attackData.aoeRadius       = ad.value("aoeradius",       0.f);
    data.attackData.nbTarget        = ad.value("nbtarget",        1);

    if (node.contains("paths")) {
        data.buildingSpritePath   = node["paths"].value("building",   "");
        data.projectileSpritePath = node["paths"].value("projectile", "");
    }

    // ─────────────────────────────────────────────
    // Chargement texture bâtiment
    // ─────────────────────────────────────────────
    {
        sf::Texture tex;
        if (!data.buildingSpritePath.empty() && tex.loadFromFile(data.buildingSpritePath)) {
            buildingTextures_[name] = std::move(tex);
        } else {
            std::cerr << "[TowerController] Missing building sprite for '"
                      << name << "': " << data.buildingSpritePath << "\n";

            sf::Image img({32, 32}, sf::Color(180, 180, 180));
            (void)buildingTextures_[name].loadFromImage(img); // ✔️ warning SFML3 supprimé
        }
    }

    // ─────────────────────────────────────────────
    // Chargement texture projectile
    // ─────────────────────────────────────────────
    {
        sf::Texture tex;
        if (!data.projectileSpritePath.empty() && tex.loadFromFile(data.projectileSpritePath)) {
            projectileTextures_[name] = std::move(tex);
        } else {
            std::cerr << "[TowerController] Missing projectile sprite for '"
                      << name << "': " << data.projectileSpritePath << "\n";

            sf::Image img({8, 8}, sf::Color(255, 220, 0));
            (void)projectileTextures_[name].loadFromImage(img); // ✔️ warning SFML3 supprimé
        }
    }

    towerDefs_[name] = std::move(data);
    std::cout << "[TowerController] Registered: " << name << "\n";
}

// ─────────────────────────────────────────────
// Accesseurs
// ─────────────────────────────────────────────
const sf::Texture* TowerController::getBuildingTexture(const std::string& name) const {
    auto it = buildingTextures_.find(name);
    return (it != buildingTextures_.end()) ? &it->second : nullptr;
}

const sf::Texture* TowerController::getProjectileTexture(const std::string& name) const {
    auto it = projectileTextures_.find(name);
    return (it != projectileTextures_.end()) ? &it->second : nullptr;
}

const TowerData* TowerController::getTowerData(const std::string& name) const {
    auto it = towerDefs_.find(name);
    return (it != towerDefs_.end()) ? &it->second : nullptr;
}

// ─────────────────────────────────────────────
// Sélection
// ─────────────────────────────────────────────
void TowerController::selectTower(const std::string& name) {
    if (towerDefs_.count(name)) {
        selected_ = name;
        std::cout << "[TowerController] Selected: " << name << "\n";
    } else {
        std::cerr << "[TowerController] Unknown tower: " << name << "\n";
    }
}

void TowerController::deselect() {
    selected_.reset();
}

bool TowerController::hasSelection() const {
    return selected_.has_value();
}

const std::string& TowerController::selectedName() const {
    static const std::string empty;
    return selected_ ? *selected_ : empty;
}

// ─────────────────────────────────────────────
// placeTower
// ─────────────────────────────────────────────
bool TowerController::placeTower(sf::Vector2f worldPos) {
    if (!selected_) return false;

    const std::string& name = *selected_;
    const sf::Texture* tex  = getBuildingTexture(name);
    if (!tex) return false;

    const auto& def = towerDefs_[name];

    PlacedTower pt(def.id, worldPos, *tex);

    sf::FloatRect bounds = pt.sprite.getLocalBounds();
    pt.sprite.setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f));
    pt.sprite.setPosition(worldPos);

    placedTowers_.push_back(std::move(pt));

    std::cout << "[TowerController] Placed '" << name
              << "' at (" << worldPos.x << ", " << worldPos.y << ")\n";
    return true;
}

// ─────────────────────────────────────────────
// drawGhost
// ─────────────────────────────────────────────
void TowerController::drawGhost(sf::RenderWindow& window, sf::Vector2f mousePos) const {
    if (!selected_) return;
    const sf::Texture* tex = getBuildingTexture(*selected_);
    if (!tex) return;

    sf::Sprite ghost(*tex);
    sf::FloatRect bounds = ghost.getLocalBounds();
    ghost.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
    ghost.setPosition(mousePos);
    ghost.setColor(sf::Color(255, 255, 255, 160));
    window.draw(ghost);
}

// ─────────────────────────────────────────────
// drawPlaced
// ─────────────────────────────────────────────
void TowerController::drawPlaced(sf::RenderWindow& window) const {
    for (const auto& pt : placedTowers_)
        window.draw(pt.sprite);
}
