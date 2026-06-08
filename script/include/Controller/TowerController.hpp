#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <string>

#include "Model/Map.hpp"
#include "Model/Tower.hpp"
#include "Model/Projectile.hpp"
#include "Model/Enemy.hpp"

class TowerController {
public:
    TowerController();

    bool loadFromJson(const std::string& path);

    // Map reference needed for placement validation and snapping
    void setMap(const Map* map, float mapScale);

    // ─── Selection / placement
    bool        selectTower(const std::string& type);
    void        clearSelection();
    bool        placeTower(sf::Vector2f pos);
    std::string getSelectedType() const { return m_selectedType; }

    // ─── Upgrade / Sell
    int  getTowerIndexAt(sf::Vector2f pos) const;
    void selectTowerForUpgrade(int index);
    bool upgradeTower(const std::string& lv2Type);
    bool sellSelectedTower();

    // ─── State
    bool hasSelection()     const { return !m_selectedType.empty(); }
    bool hasUpgradeTarget() const { return m_upgradeTargetIndex >= 0; }
    int  getCoins()         const { return m_coins; }
    void addCoins(int amount)     { m_coins += amount; }
    int  getCostOf(const std::string& type) const;

    // ─── Ghost / hover cell queries
    void         setGhostPosition(sf::Vector2f pos);
    sf::Vector2f getSnappedPosition(sf::Vector2f pos) const;
    bool         isValidPlacement(sf::Vector2f pos) const;
    sf::FloatRect getHoverCellRect(sf::Vector2f pos) const;

    // Returns placed tower centers (for overlay rendering)
    std::vector<sf::Vector2f> getTowerPositions() const;

    // Returns the world position of the currently selected tower (-1 guard in impl)
    sf::Vector2f getSelectedTowerPosition() const;

    // ─── Loop
    void update(float dt, const std::vector<std::shared_ptr<Enemy>>& enemies);
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

    std::map<std::string, TowerDef> m_defs;

    std::vector<Tower>      m_towers;
    std::vector<Projectile> m_projectiles;

    std::string               m_selectedType;
    int                       m_upgradeTargetIndex = -1;

    std::optional<sf::Sprite> m_ghost;
    bool                      m_ghostVisible = false;

    int m_coins = 1;

    sf::SoundBuffer m_basicShot;
    sf::SoundBuffer m_fireShot;
    sf::SoundBuffer m_iceShot;
    sf::SoundBuffer m_rockShot;
    sf::SoundBuffer m_arcaneShot;

    std::vector<sf::Sound> m_tempSounds;

    const Map* m_map      = nullptr;
    float      m_mapScale = 2.f;

    void spawnGhost(const std::string& type);
    void updateGhostColor(sf::Vector2f pos);

    static constexpr float SELL_REFUND = 0.5f;
};