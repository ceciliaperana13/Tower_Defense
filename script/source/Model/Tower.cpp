#include "Model/Tower.hpp"

Tower::Tower(const sf::Texture& buildingTex,
             const sf::Texture& projectileTex,
             const AttackData& attack,
             sf::Vector2f pos,
             int cost)
    : m_sprite(buildingTex)
    , m_projectileTex(projectileTex)
    , m_attack(attack)
    , m_position(pos)
    , m_cost(cost)
{
    m_sprite.setOrigin(sf::Vector2f(
        buildingTex.getSize().x / 2.f,
        buildingTex.getSize().y / 2.f
    ));

    m_sprite.setScale(sf::Vector2f(2.f, 2.f));
    m_sprite.setPosition(pos);
}

void Tower::update(float dt)
{
    if (m_fireCooldown > 0.f)
        m_fireCooldown -= dt;
}

bool Tower::canFire() const
{
    return m_fireCooldown <= 0.f;
}

void Tower::resetCooldown()
{
    m_fireCooldown = 1.f / m_attack.fireRate;
}

void Tower::render(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}