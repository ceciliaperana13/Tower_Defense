#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>

#include "Enemy.hpp"

// ─────────────────────────────────────────────
// Données d’attaque d’une tour
// ─────────────────────────────────────────────
struct AttackData {
    int   damage = 1;
    float range = 1.f;
    float fireRate = 1.f;
    float damagePerSecond = 0.f;
    float slowness = 0.f;
    float effectDuration = 0.f;
    float aoeRadius = 0.f;
    int   nbTarget = 1;
};

// ─────────────────────────────────────────────
// Définition d’une tour (depuis JSON)
// ─────────────────────────────────────────────
struct TowerDef {
    int         id = 0;
    AttackData  attack;
    std::string buildingPath;
    std::string projectilePath;   // pour basic = normal_projectile.png
};

// ─────────────────────────────────────────────
// Projectile tiré par une tour
// ─────────────────────────────────────────────
struct Projectile {
    sf::Sprite  sprite;
    sf::Vector2f velocity;
    float       lifeTime = 0.f;
    int         damage   = 1;

    Projectile(const sf::Texture& tex)
        : sprite(tex) {}
};

// ─────────────────────────────────────────────
// Instance d’une tour placée sur la map
// ─────────────────────────────────────────────
struct Tower {
    sf::Sprite   sprite;
    sf::Vector2f position;
    AttackData   attack;
    float        fireCooldown = 0.f;

    Tower(const sf::Texture& tex)
        : sprite(tex) {}
};

// ─────────────────────────────────────────────
// TowerController — gère :
// ✔ sélection de tour
// ✔ ghost
// ✔ placement
// ✔ tir
// ✔ projectiles
// ─────────────────────────────────────────────
class TowerController {
public:
    TowerController();

    // Charge towers.json
    bool loadFromJson(const std::string& path);

    // Sélectionne la tour basic
    void selectBasic();

    // Désélectionne
    void clearSelection();

    // Déplace le ghost
    void setGhostPosition(sf::Vector2f worldPos);

    // Place la tour sélectionnée
    void placeCurrentTower(sf::Vector2f worldPos);

    // Update logique (tir + projectiles)
    void update(float dt,
                const std::vector<std::unique_ptr<Enemy>>& enemies);

    // Affichage
    void render(sf::RenderWindow& window);

    bool hasSelection() const { return m_hasSelection; }

private:
    // Définition de la tour basic (depuis JSON)
    TowerDef m_basicDef;

    // Textures
    sf::Texture m_basicTex;             // sprite tour basic
    sf::Texture m_basicProjectileTex;   // sprite projectile basic (normal_projectile.png)

    // Tours placées
    std::vector<Tower> m_towers;

    // Projectiles tirés
    std::vector<Projectile> m_projectiles;

    // Ghost
    bool        m_hasSelection = false;
    bool        m_ghostVisible = false;
    sf::Sprite  m_ghostSprite;

private:
    // Trouve une cible dans la portée
    Enemy* findTarget(const Tower& tower,
                      const std::vector<std::unique_ptr<Enemy>>& enemies);

    // Tire un projectile
    void fireFromTower(Tower& tower,
                       const std::vector<std::unique_ptr<Enemy>>& enemies,
                       float dt);

    // Update projectiles
    void updateProjectiles(float dt,
                           const std::vector<std::unique_ptr<Enemy>>& enemies);
};
