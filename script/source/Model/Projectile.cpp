#include "Model/Projectile.hpp"
#include <cmath>

static float len(sf::Vector2f v) {
    return std::sqrt(v.x*v.x + v.y*v.y);
}

Projectile::Projectile(const sf::Texture& tex,
                       sf::Vector2f start,
                       Enemy* target,
                       const std::string& soundPath,
                       int damage)
    : m_sprite(tex)
    , m_target(target)
    , m_soundPath(soundPath)
    , m_damage(damage)
{
    m_sprite.setOrigin({
        tex.getSize().x / 2.f,
        tex.getSize().y / 2.f
    });
    m_sprite.setScale({ 2.f, 2.f });
    m_sprite.setPosition(start);

    // Initial direction toward target center
    sf::Vector2f dir = enemyCenter() - start;
    float d = len(dir);
    if (d > 0.f) dir /= d;
    m_velocity = dir * 280.f;
}

// Returns the center of the tracked enemy, or last known position if dead.
sf::Vector2f Projectile::enemyCenter() const {
    if (!m_target || m_target->isDead() || m_target->hasReached())
        return m_sprite.getPosition() + m_velocity * 0.1f; // keep going forward

    sf::FloatRect b = m_target->getBounds();
    return { b.position.x + b.size.x / 2.f,
             b.position.y + b.size.y / 2.f };
}

void Projectile::update(float dt) {
    if (m_dead) return;

    m_life -= dt;
    if (m_life <= 0.f) { m_dead = true; return; }

    // Steer toward current enemy center every frame
    if (m_target && !m_target->isDead() && !m_target->hasReached()) {
        sf::Vector2f dir = enemyCenter() - m_sprite.getPosition();
        float d = len(dir);
        if (d > 0.f) {
            m_velocity = (dir / d) * 280.f;
        }
    }

    m_sprite.move(m_velocity * dt);
    checkCollision();
}

bool Projectile::checkCollision() {
    if (m_dead || !m_target || m_target->isDead() || m_target->hasReached())
        return false;

    sf::Vector2f center = enemyCenter();
    sf::Vector2f proj   = m_sprite.getPosition();
    float dx = proj.x - center.x;
    float dy = proj.y - center.y;

    constexpr float HIT_RADIUS = 12.f;
    if (dx*dx + dy*dy <= HIT_RADIUS * HIT_RADIUS) {
        m_target->takeDamage(m_damage);
        SoundManager::getInstance().loadSFX(m_soundPath);
        SoundManager::getInstance().playSFX(m_soundPath);
        m_dead = true;
        return true;
    }
    return false;
}

void Projectile::render(sf::RenderWindow& window) {
    if (!m_dead)
        window.draw(m_sprite);
}