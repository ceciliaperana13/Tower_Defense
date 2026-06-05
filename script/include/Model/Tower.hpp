#pragma once

#include <SFML/Graphics.hpp>
#include "Model/Enemy.hpp"
#include <string>

struct AttackData {
    int damage;
    float range;
    float fireRate;
    float damagePerSecond;
    float slowness;
    float effectDuration;
    float aoeRadius;
    int nbTarget;
};

class Tower {
public:
    Tower(const sf::Texture& buildingTex,
          const sf::Texture& projectileTex,
          const AttackData& attack,
          const std::string soundPath,
          sf::Vector2f pos,
          int cost);

    void update(float dt);
    void render(sf::RenderWindow& window);

    bool canFire() const;
    void resetCooldown();

    sf::Vector2f getPosition() const { return m_position; }
    const AttackData& getAttack() const { return m_attack; }
    const sf::Texture& getProjectileTexture() const { return m_projectileTex; }
    const std::string getSoundPath() const { return m_soundPath; }

    int getCost() const { return m_cost; }

private:
    sf::Sprite m_sprite;
    sf::Texture m_projectileTex;
    AttackData m_attack;
    sf::Vector2f m_position;
    std::string m_soundPath;

    float m_fireCooldown = 0.f;
    int m_cost = 0;
};