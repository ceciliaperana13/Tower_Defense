#include "Projectile.hpp"
#include <cmath>

static float length(sf::Vector2f v) {
    return std::sqrt(v.x*v.x + v.y*v.y);
}

Projectile::Projectile(const sf::Texture& tex,
                       sf::Vector2f start,
                       sf::Vector2f target,
                       int damage, int id)
    : m_sprite(tex)          
    , m_damage(damage)
    , m_id(id)
{
    m_sprite.setOrigin(sf::Vector2f(
        tex.getSize().x / 2.f,
        tex.getSize().y / 2.f
    ));
    m_sprite.setScale(sf::Vector2f(2.f, 2.f));
    m_sprite.setPosition(start);

    sf::Vector2f dir = target - start;
    float len = length(dir);
    if (len > 0.f) dir /= len;

    m_velocity = dir * 250.f;
}

void Projectile::update(float dt) {
    m_life -= dt;
    m_sprite.move(m_velocity * dt);
}

bool Projectile::checkCollision(Enemy& e) {
    auto inter = m_sprite.getGlobalBounds().findIntersection(e.getBounds());
    if (inter.has_value()) {
        e.takeDamage(m_damage);
        SoundManager::getInstance().playProjectileImpact(m_id);
        m_life = 0.f;
        return true;
    }
    return false;
}

void Projectile::render(sf::RenderWindow& window) {
    window.draw(m_sprite);
}
