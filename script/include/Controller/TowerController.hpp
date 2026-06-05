#pragma once
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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
    std::string getSelectedType() const { return m_selectedType; }

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
        std::string soundPath;
        sf::Texture buildingTex;
        sf::Texture projectileTex;
    };

    // Définitions des tours
    std::map<std::string, TowerDef> m_defs;

    // Tours et projectiles
    std::vector<Tower>      m_towers;
    std::vector<Projectile> m_projectiles;

    // Sélection
    std::string               m_selectedType;
    int                       m_upgradeTargetIndex = -1;

    // Ghost
    std::optional<sf::Sprite> m_ghost;
    bool                      m_ghostVisible = false;

    // Coins
    int m_coins = 1;

    // Sons de tir
    sf::SoundBuffer m_basicShot;
    sf::SoundBuffer m_fireShot;
    sf::SoundBuffer m_iceShot;
    sf::SoundBuffer m_rockShot;
    sf::SoundBuffer m_arcaneShot;

    // Sons en cours (pour éviter qu'ils soient détruits)
    std::vector<sf::Sound> m_tempSounds;

    // Ghost
    void spawnGhost(const std::string& type);

    // Refund
    static constexpr float SELL_REFUND = 0.5f;
};
