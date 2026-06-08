#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Model/Enemy.hpp"
#include "Model/Tower.hpp"
#include "../Controller/SoundManager.hpp"

class Projectile {
public:
    Projectile(const sf::Texture& tex,
               sf::Vector2f       start,
               std::weak_ptr<Enemy> target,   // weak_ptr: safe when enemy dies
               const std::string& soundPath,
               const AttackData&  attack,
               const std::vector<std::shared_ptr<Enemy>>* allEnemies = nullptr);

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
    std::weak_ptr<Enemy> m_target;   // weak_ptr: does not prevent destruction
    std::string  m_soundPath;
    AttackData   m_attack;
    const std::vector<std::shared_ptr<Enemy>>* m_allEnemies;

    bool  m_dead { false };
    float m_life { 4.f };
};