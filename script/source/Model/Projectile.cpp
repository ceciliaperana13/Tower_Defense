#include "Model/Projectile.hpp"
#include <cmath>
#include <memory>

static float vlen(sf::Vector2f v) {
    return std::sqrt(v.x*v.x + v.y*v.y);
}

Projectile::Projectile(const sf::Texture& tex,
                       sf::Vector2f       start,
                       Enemy*             target,
                       const std::string& soundPath,
                       const AttackData&  attack,
                       const std::vector<std::unique_ptr<Enemy>>* allEnemies)
    : m_sprite(tex)
    , m_target(target)
    , m_soundPath(soundPath)
    , m_attack(attack)
    , m_allEnemies(allEnemies)
{
    m_sprite.setOrigin({ tex.getSize().x / 2.f, tex.getSize().y / 2.f });
    m_sprite.setScale({ 2.f, 2.f });
    m_sprite.setPosition(start);

    sf::Vector2f dir = enemyCenter() - start;
    float d = vlen(dir);
    if (d > 0.f) dir /= d;
    m_velocity = dir * 300.f;
}

sf::Vector2f Projectile::enemyCenter() const {
    if (!m_target || m_target->isDead() || m_target->hasReached())
        return m_sprite.getPosition() + m_velocity * 0.1f;
    sf::FloatRect b = m_target->getBounds();
    return { b.position.x + b.size.x / 2.f,
             b.position.y + b.size.y / 2.f };
}

void Projectile::update(float dt) {
    if (m_dead) return;
    m_life -= dt;
    if (m_life <= 0.f) { m_dead = true; return; }

    // Track enemy every frame
    if (m_target && !m_target->isDead() && !m_target->hasReached()) {
        sf::Vector2f dir = enemyCenter() - m_sprite.getPosition();
        float d = vlen(dir);
        if (d > 0.f) m_velocity = (dir / d) * 300.f;
    }

    m_sprite.move(m_velocity * dt);
    checkCollision();
}

void Projectile::checkCollision() {
    if (m_dead || !m_target || m_target->isDead() || m_target->hasReached())
        return;

    sf::Vector2f center = enemyCenter();
    sf::Vector2f proj   = m_sprite.getPosition();
    float dx = proj.x - center.x;
    float dy = proj.y - center.y;

    constexpr float HIT_RADIUS = 12.f;
    if (dx*dx + dy*dy > HIT_RADIUS * HIT_RADIUS) return;

    SoundManager::getInstance().loadSFX(m_soundPath);
    SoundManager::getInstance().playSFX(m_soundPath);

    if (m_attack.aoeRadius > 0.f) {
        // Rock: AoE splash — hits all enemies within aoeRadius tiles
        applySplash(center);
    } else {
        // Direct hit on the tracked target + any status effect
        applyEffects(*m_target);
    }

    m_dead = true;
}

// Applies direct damage + status effect to a single enemy.
void Projectile::applyEffects(Enemy& e) {
    e.takeDamage(m_attack.damage);

    if (m_attack.damagePerSecond > 0.f && m_attack.effectDuration > 0.f)
        e.applyBurn(m_attack.damagePerSecond, m_attack.effectDuration);

    if (m_attack.slowness > 0.f && m_attack.effectDuration > 0.f)
        e.applySlow(1.f - m_attack.slowness, m_attack.effectDuration);
}

// Rock AoE: damages all enemies within aoeRadius tiles of the impact point.
void Projectile::applySplash(sf::Vector2f hitPos) {
    if (!m_allEnemies) { applyEffects(*m_target); return; }

    float radiusPx = m_attack.aoeRadius * 64.f; // tiles → pixels (scale 2 * 32px)
    for (const auto& e : *m_allEnemies) {
        if (e->isDead() || e->hasReached()) continue;
        sf::FloatRect b = e->getBounds();
        sf::Vector2f ec = { b.position.x + b.size.x / 2.f,
                            b.position.y + b.size.y / 2.f };
        float dx = ec.x - hitPos.x;
        float dy = ec.y - hitPos.y;
        if (dx*dx + dy*dy <= radiusPx * radiusPx)
            applyEffects(*e);
    }
}

void Projectile::render(sf::RenderWindow& window) {
    if (!m_dead)
        window.draw(m_sprite);
}