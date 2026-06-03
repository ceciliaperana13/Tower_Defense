#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <optional>
#include <vector>
#include <json.hpp>

// ─────────────────────────────────────────────
// AttackData
// ─────────────────────────────────────────────
struct AttackData {
    float damage         = 0.f;
    float range          = 0.f;
    float fireRate       = 0.f;
    float damagePerSecond= 0.f;
    float slowness       = 0.f;
    float effectDuration = 0.f;
    float aoeRadius      = 0.f;
    int   nbTarget       = 1;
};

// ─────────────────────────────────────────────
// TowerData : données brutes lues depuis le JSON
// ─────────────────────────────────────────────
struct TowerData {
    int         id   = 0;
    std::string name;
    AttackData  attackData;
    std::string buildingSpritePath;
    std::string projectileSpritePath;
};

// ─────────────────────────────────────────────
// PlacedTower : une tour posée sur la carte
// ─────────────────────────────────────────────
struct PlacedTower {
    int          dataId = 0;
    sf::Vector2f position;
    sf::Sprite   sprite;

    PlacedTower(int id, sf::Vector2f pos, const sf::Texture& tex)
        : dataId(id),
          position(pos),
          sprite(tex) {}
};

// ─────────────────────────────────────────────
// TowerController
// ─────────────────────────────────────────────
class TowerController {
public:
    TowerController() = default;

    // Charge towers.json et tous les sprites associés
    bool loadFromJson(const std::string& jsonPath);

    // Accesseurs textures / données
    const sf::Texture* getBuildingTexture   (const std::string& name) const;
    const sf::Texture* getProjectileTexture (const std::string& name) const;
    const TowerData*   getTowerData         (const std::string& name) const;

    // Sélection
    void               selectTower (const std::string& name);
    void               deselect    ();
    bool               hasSelection() const;
    const std::string& selectedName() const;

    // Placement
    bool placeTower(sf::Vector2f worldPos);

    // Rendu
    void drawGhost  (sf::RenderWindow& window, sf::Vector2f mousePos) const;
    void drawPlaced (sf::RenderWindow& window)                         const;

    // Accès lecture seule à la liste des tours placées
    const std::vector<PlacedTower>& getPlacedTowers() const { return placedTowers_; }

private:
    std::unordered_map<std::string, TowerData>   towerDefs_;
    std::unordered_map<std::string, sf::Texture> buildingTextures_;
    std::unordered_map<std::string, sf::Texture> projectileTextures_;

    std::optional<std::string> selected_;
    std::vector<PlacedTower>   placedTowers_;

    void parseTowerEntry(const std::string& name, const nlohmann::json& node);
};