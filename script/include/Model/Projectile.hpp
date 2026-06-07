#pragma once
#include <SFML/Graphics.hpp>
#include "Model/Enemy.hpp"
#include "../Controller/SoundManager.hpp"

class Projectile {
public:
    Projectile(const sf::Texture& tex,
               sf::Vector2f start,
               Enemy* target,       // tracks the enemy until hit or dead
               const std::string& soundPath,
               int damage);

    void update(float dt);
    void render(sf::RenderWindow& window);
    bool isDead() const { return m_dead; }

    bool checkCollision();

private:
    sf::Vector2f enemyCenter() const;

    sf::Sprite   m_sprite;
    sf::Vector2f m_velocity;    // fallback direction when target is gone
    Enemy*       m_target;
    std::string  m_soundPath;
    int          m_damage;
    bool         m_dead = false;
    float        m_maxLife = 4.f; // safety TTL
    float        m_life    = 4.f;
};