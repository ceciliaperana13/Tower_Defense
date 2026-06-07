#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Model/Enemy.hpp"
#include "Model/Tower.hpp"
#include "../Controller/SoundManager.hpp"

// Carries all attack parameters so it can apply the right effect on impact.
class Projectile {
public:
    Projectile(const sf::Texture& tex,
               sf::Vector2f       start,
               Enemy*             target,
               const std::string& soundPath,
               const AttackData&  attack,
               // For AoE: pointer to full enemy list so we can splash on hit
               const std::vector<std::unique_ptr<Enemy>>* allEnemies = nullptr);

    void update(float dt);
    void render(sf::RenderWindow& window);
    bool isDead() const { return m_dead; }

private:
    void checkCollision();
    void applyEffects(Enemy& hit);
    void applySplash(sf::Vector2f hitPos);
    sf::Vector2f enemyCenter() const;

    sf::Sprite   m_sprite;
    sf::Vector2f m_velocity;
    Enemy*       m_target;
    std::string  m_soundPath;
    AttackData   m_attack;
    const std::vector<std::unique_ptr<Enemy>>* m_allEnemies;

    bool  m_dead { false };
    float m_life { 4.f };
};