#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Tower.hpp"
#include "Projectile.hpp"
#include <map>
#include <optional>
#include "Enemy.hpp"
#include <string>



class TowerController {
public:
    TowerController();

    bool loadFromJson(const std::string& path);

    void selectTower(const std::string& type);
    void placeTower(sf::Vector2f pos);

    void update(float dt, const std::vector<std::unique_ptr<Enemy>>& enemies);
    void render(sf::RenderWindow& window);

    void setGhostPosition(sf::Vector2f pos);
    bool hasSelection() const { return m_selectedType != ""; }

private:
    struct TowerDef {
        int id;
        AttackData attack;
        std::string buildingPath;
        std::string projectilePath;
        sf::Texture buildingTex;
        sf::Texture projectileTex;
    };

    std::map<std::string, TowerDef> m_defs;

    std::vector<Tower> m_towers;
    std::vector<Projectile> m_projectiles;

    std::string m_selectedType;

    std::optional<sf::Sprite> m_ghost;

    bool m_ghostVisible = false;
};
