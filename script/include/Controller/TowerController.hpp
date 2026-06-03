#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <string>
#include <functional>

#include "Tower.hpp"
#include "Projectile.hpp"
#include "Enemy.hpp"

class TowerController {
public:
    TowerController();

    bool loadFromJson(const std::string& path);

    // ── Sélection / placement ────────────────────────────────────────────────
    // Sélectionne un type de tour (vérifie les coins).
    // Retourne false si pas assez de coins.
    bool selectTower(const std::string& type);

    // Surcharge sans vérif coins (pour désélectionner avec "")
    void clearSelection();

    // Place la tour sélectionnée à pos, déduit les coins.
    void placeTower(sf::Vector2f pos);

    // ── Upgrade ──────────────────────────────────────────────────────────────
    // Retourne l'index de la tour cliquée (-1 si aucune)
    int  getTowerIndexAt(sf::Vector2f pos) const;

    // Sélectionne une tour existante pour upgrade
    void selectTowerForUpgrade(int index);

    // Upgrade la tour sélectionnée vers le type lv2 donné
    // Retourne false si pas assez de coins ou tour déjà lv2
    bool upgradeTower(const std::string& lv2Type);

    // ── État ─────────────────────────────────────────────────────────────────
    bool hasSelection()     const { return !m_selectedType.empty(); }
    bool hasUpgradeTarget() const { return m_upgradeTargetIndex >= 0; }
    int  getCoins()         const { return m_coins; }
    void addCoins(int amount)     { m_coins += amount; }
    int  getCostOf(const std::string& type) const;

    // ── Ghost ────────────────────────────────────────────────────────────────
    void setGhostPosition(sf::Vector2f pos);

    // ── Boucle ───────────────────────────────────────────────────────────────
    void update(float dt, const std::vector<std::unique_ptr<Enemy>>& enemies);
    void render(sf::RenderWindow& window);

private:
    struct TowerDef {
        int         id;
        int         cost;
        std::string level;   // "lvl1" ou "lvl2"
        AttackData  attack;
        std::string buildingPath;
        std::string projectilePath;
        sf::Texture buildingTex;
        sf::Texture projectileTex;
    };

    std::map<std::string, TowerDef> m_defs;

    std::vector<Tower>      m_towers;
    std::vector<Projectile> m_projectiles;

    std::string m_selectedType;          // type en cours de placement
    int         m_upgradeTargetIndex = -1; // index tour sélectionnée pour upgrade

    std::optional<sf::Sprite> m_ghost;
    bool m_ghostVisible = false;

    int m_coins = 100;  // coins de départ

    void spawnGhost(const std::string& type);
};