#include "Model/Projectile.hpp"
#include <cmath>

static float vlen(sf::Vector2f v) {
    return std::sqrt(v.x*v.x + v.y*v.y);
}

Projectile::Projectile(const sf::Texture& tex,
                       sf::Vector2f       start,
                       std::weak_ptr<Enemy> target,
                       const std::string& soundPath,
                       const AttackData&  attack,
                       const std::vector<std::shared_ptr<Enemy>>* allEnemies)
    : m_sprite(tex)
    , m_target(target)
    , m_soundPath(soundPath)
    , m_attack(attack)
    , m_allEnemies(allEnemies)
{
    m_sprite.setOrigin({ tex.getSize().x / 2.f, tex.getSize().y / 2.f });
    m_sprite.setScale({ 2.f, 2.f });
    m_sprite.setPosition(start);

    // Initial direction — lock the shared_ptr briefly just for construction
    sf::Vector2f dir = enemyCenter() - start;
    float d = vlen(dir);
    if (d > 0.f) dir /= d;
    m_velocity = dir * 300.f;
}

sf::Vector2f Projectile::enemyCenter() const {
    auto locked = m_target.lock();
    if (!locked || locked->isDead() || locked->hasReached())
        return m_sprite.getPosition(); // target gone: stay put (will die next frame)
    sf::FloatRect b = locked->getBounds();
    return { b.position.x + b.size.x / 2.f,
             b.position.y + b.size.y / 2.f };
}

void Projectile::update(float dt) {
    if (m_dead) return;

    // Kill projectile immediately if target is gone
    auto locked = m_target.lock();
    if (!locked || locked->isDead() || locked->hasReached()) {
        m_dead = true;
        return;
    }

    m_life -= dt;
    if (m_life <= 0.f) { m_dead = true; return; }

    // Track current enemy center every frame
    sf::Vector2f dir = enemyCenter() - m_sprite.getPosition();
    float d = vlen(dir);
    if (d > 0.f) m_velocity = (dir / d) * 300.f;

    m_sprite.move(m_velocity * dt);
    checkCollision();
}

void Projectile::checkCollision() {
    if (m_dead) return;
    auto locked = m_target.lock();
    if (!locked || locked->isDead() || locked->hasReached()) {
        m_dead = true;
        return;
    }

    sf::Vector2f center = enemyCenter();
    sf::Vector2f proj   = m_sprite.getPosition();
    float dx = proj.x - center.x;
    float dy = proj.y - center.y;

    constexpr float HIT_RADIUS = 12.f;
    if (dx*dx + dy*dy > HIT_RADIUS * HIT_RADIUS) return;

    SoundManager::getInstance().loadSFX(m_soundPath);
    SoundManager::getInstance().playSFX(m_soundPath);

    if (m_attack.aoeRadius > 0.f)
        applySplash(center);
    else
        applyEffects(*locked);

    m_dead = true;
}

void Projectile::applyEffects(Enemy& e) {
    e.takeDamage(m_attack.damage);
    if (m_attack.damagePerSecond > 0.f && m_attack.effectDuration > 0.f)
        e.applyBurn(m_attack.damagePerSecond, m_attack.effectDuration);
    if (m_attack.slowness > 0.f && m_attack.effectDuration > 0.f)
        e.applySlow(1.f - m_attack.slowness, m_attack.effectDuration);
}

void Projectile::applySplash(sf::Vector2f hitPos) {
    if (!m_allEnemies) {
        auto locked = m_target.lock();
        if (locked) applyEffects(*locked);
        return;
    }
    float radiusPx = m_attack.aoeRadius * 64.f;
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