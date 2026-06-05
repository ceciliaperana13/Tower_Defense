#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <string>

#include "Model/Tower.hpp"
#include "Model/Projectile.hpp"
#include "Model/Enemy.hpp"

class TowerController {
public:
    TowerController();

    bool loadFromJson(const std::string& path);

    // ─── Sélection / placement
    bool        selectTower(const std::string& type);
    void        clearSelection();
    void        placeTower(sf::Vector2f pos);
    std::string getSelectedType() const { return m_selectedType; } // ← nouveau

    // ─── Upgrade / Sell
    int  getTowerIndexAt(sf::Vector2f pos) const;
    void selectTowerForUpgrade(int index);
    bool upgradeTower(const std::string& lv2Type);
    bool sellSelectedTower();

    // ─── État
    bool hasSelection()     const { return !m_selectedType.empty(); }
    bool hasUpgradeTarget() const { return m_upgradeTargetIndex >= 0; }
    int  getCoins()         const { return m_coins; }
    void addCoins(int amount)     { m_coins += amount; }
    int  getCostOf(const std::string& type) const;

    // ─── Ghost
    void setGhostPosition(sf::Vector2f pos);

    // ─── Boucle
    void update(float dt, const std::vector<std::unique_ptr<Enemy>>& enemies);
    void render(sf::RenderWindow& window);

private:
    struct TowerDef {
        int         id;
        int         cost;
        std::string level;
        AttackData  attack;
        std::string buildingPath;
        std::string projectilePath;
        sf::Texture buildingTex;
        sf::Texture projectileTex;
    };

    std::map<std::string, TowerDef> m_defs;
    std::vector<Tower>              m_towers;
    std::vector<Projectile>         m_projectiles;

    std::string               m_selectedType;
    int                       m_upgradeTargetIndex = -1;
    std::optional<sf::Sprite> m_ghost;
    bool                      m_ghostVisible = false;
    int                       m_coins        = 1;

    void spawnGhost(const std::string& type);

    static constexpr float SELL_REFUND = 0.5f;
};