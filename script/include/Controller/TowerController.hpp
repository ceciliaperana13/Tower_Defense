#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>
#include "Towers.hpp"

// ─────────────────────────────────────────────
// TowerData : données brutes lues depuis le JSON
// ─────────────────────────────────────────────
struct AttackData {
    float damage;
    float range;
    float fireRate;
    float damagePerSecond;
    float slowness;
    float effectDuration;
    float aoeRadius;
    int   nbTarget;
};

struct TowerData {
    int         id;
    std::string name;       // "basic", "fire", "ice"…
    AttackData  attackData;
    std::string buildingSpritePath;
    std::string projectileSpritePath;
};

// ─────────────────────────────────────────────
// PlacedTower : une tour posée sur la carte
// ─────────────────────────────────────────────
struct PlacedTower {
    int        dataId;          // référence vers TowerData::id
    sf::Sprite sprite;
    sf::Vector2f position;
};

// ─────────────────────────────────────────────
// TowerController
// ─────────────────────────────────────────────
class TowerController {
public:
    TowerController();

    // Charge towers.json et tous les sprites associés
    bool loadFromJson(const std::string& jsonPath);

    // Retourne la texture du bâtiment d'une tour (par nom)
    const sf::Texture* getBuildingTexture(const std::string& name) const;
    const sf::Texture* getProjectileTexture(const std::string& name) const;

    // Retourne toutes les données d'une tour par nom
    const TowerData* getTowerData(const std::string& name) const;

    // Sélectionne une tour à placer ("basic", "fire"…). nullopt = désélection.
    void selectTower(const std::string& name);
    void deselect();
    bool hasSelection() const;
    const std::string& selectedName() const;

    // Pose la tour sélectionnée à la position donnée
    // Retourne true si le placement a réussi
    bool placeTower(sf::Vector2f worldPos);

    // Dessine le fantôme (preview) sous le curseur
    void drawGhost(sf::RenderWindow& window, sf::Vector2f mousePos) const;

    // Dessine toutes les tours posées
    void drawPlaced(sf::RenderWindow& window) const;

    // Accès à la liste des tours placées (lecture seule)
    const std::vector<PlacedTower>& getPlacedTowers() const { return placedTowers_; }

private:
    // Toutes les définitions chargées depuis le JSON (name → TowerData)
    std::unordered_map<std::string, TowerData>   towerDefs_;

    // Textures des bâtiments et projectiles (name → texture)
    std::unordered_map<std::string, sf::Texture> buildingTextures_;
    std::unordered_map<std::string, sf::Texture> projectileTextures_;

    // Tour actuellement sélectionnée pour placement
    std::optional<std::string> selected_;

    // Tours déjà posées sur la carte
    std::vector<PlacedTower> placedTowers_;

    // Helper : parse un nœud JSON "tower entry" et l'enregistre
    void parseTowerEntry(const std::string& name, const nlohmann::json& node);
};
