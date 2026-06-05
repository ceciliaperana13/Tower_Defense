#pragma once
#include <SFML/Graphics.hpp>
#include "Enemy.hpp"
#include "../Controller/SoundManager.hpp"

class Projectile {
public:
    Projectile(const sf::Texture& tex,
               sf::Vector2f start,
               sf::Vector2f target,
               const std::string path,
               int damage);

    void update(float dt);
    void render(sf::RenderWindow& window);
    bool isDead() const { return m_life <= 0.f; }

    bool checkCollision(Enemy& e);

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;
    std::string m_soundPath;
    float m_life = 2.f;
    int m_damage;
};
