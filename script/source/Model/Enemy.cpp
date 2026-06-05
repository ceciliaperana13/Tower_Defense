#include "Model/Enemy.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cmath>

using json = nlohmann::json;

// ─── Membres statiques ────────────────────────────────────────
sf::Texture Enemy::s_heartTex;
bool        Enemy::s_heartLoaded = false;

bool Enemy::loadHeartTexture(const std::string& path) {
    if (!s_heartTex.loadFromFile(path)) {
        std::cerr << "[Enemy] heart.png introuvable : " << path << "\n";
        return false;
    }
    s_heartLoaded = true;
    return true;
}

// ─── Constructeur ─────────────────────────────────────────────
Enemy::Enemy(int id, int maxhp, float maxspeed, int reward,
             const std::string& sprite1Path,
             const std::string& sprite2Path,
             const std::vector<sf::Vector2f>& waypoints)
    : m_id(id), m_maxhp(maxhp), m_hp(maxhp),
      m_maxspeed(maxspeed), m_speed(maxspeed),
      m_reward(reward),
      m_waypoints(waypoints)
{
    bool ok1 = m_tex1.loadFromFile(sprite1Path);
    bool ok2 = m_tex2.loadFromFile(sprite2Path);

    if (!ok1) std::cerr << "[Enemy] Introuvable : " << sprite1Path << "\n";
    if (!ok2) std::cerr << "[Enemy] Introuvable : " << sprite2Path << "\n";

    // Créer le sprite uniquement si la texture est chargée
    if (ok1) {
        m_sprite.emplace(m_tex1);
        m_sprite->setScale(sf::Vector2f(2.f, 2.f));
        m_sprite->setTexture(m_tex1, true);  // true = reset le rect
    }

    if (!m_waypoints.empty() && m_sprite.has_value())
        m_sprite->setPosition(m_waypoints[0]);
}

// ─── Move constructor: rebind sprite to the new texture addresses
Enemy::Enemy(Enemy&& other) noexcept
    : m_id(other.m_id), m_maxhp(other.m_maxhp), m_hp(other.m_hp),
      m_maxspeed(other.m_maxspeed), m_speed(other.m_speed),
      m_reward(other.m_reward),
      m_tex1(std::move(other.m_tex1)),
      m_tex2(std::move(other.m_tex2)),
      m_sprite(std::nullopt),
      m_animTimer(other.m_animTimer), m_useFirst(other.m_useFirst),
      m_waypoints(std::move(other.m_waypoints)),
      m_waypointIdx(other.m_waypointIdx),
      m_reached(other.m_reached)
{
    if (other.m_sprite.has_value()) {
        m_sprite.emplace(m_useFirst ? m_tex1 : m_tex2);
        m_sprite->setScale(other.m_sprite->getScale());
        m_sprite->setPosition(other.m_sprite->getPosition());
    }
}

// ─── fromJson 
Enemy Enemy::fromJson(const std::string& jsonPath,
                      const std::string& type,
                      const std::vector<sf::Vector2f>& waypoints) {
    std::ifstream file(jsonPath);
    if (!file.is_open())
        throw std::runtime_error("[Enemy] Impossible d'ouvrir : " + jsonPath);

    json data = json::parse(file);

    if (!data["enemytype"].contains(type))
        throw std::runtime_error("[Enemy] Type inconnu : " + type);

    const auto& e = data["enemytype"][type];

    return Enemy(
        e["id"].get<int>(),
        e["maxhp"].get<int>(),
        e["maxspeed"].get<float>(),
        e["reward"].get<int>(),
        e["paths"]["sprite1"].get<std::string>(),
        e["paths"]["sprite2"].get<std::string>(),
        waypoints
    );
}

// ─── applyHpMultiplier
void Enemy::applyHpMultiplier(float multiplier) {
    m_maxhp = static_cast<int>(m_maxhp * multiplier);
    m_hp    = m_maxhp;
}

// ─── takeDamage
void Enemy::takeDamage(int dmg) {
    m_hp = std::max(0, m_hp - dmg);
}

// ─── moveAlongPath 
void Enemy::moveAlongPath(float dt) {
    if (m_reached || m_waypointIdx >= (int)m_waypoints.size())
        return;

    sf::Vector2f pos    = m_sprite->getPosition();
    sf::Vector2f target = m_waypoints[m_waypointIdx];
    sf::Vector2f delta  = target - pos;

    float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    float step = m_speed * dt;

    if (step >= dist) {
        m_sprite->setPosition(target);
        ++m_waypointIdx;

        if (m_waypointIdx >= (int)m_waypoints.size()) {
            m_reached = true;
        } else {
            float remaining        = step - dist;
            sf::Vector2f next      = m_waypoints[m_waypointIdx];
            sf::Vector2f nextDelta = next - target;
            float nextDist = std::sqrt(nextDelta.x * nextDelta.x + nextDelta.y * nextDelta.y);
            if (nextDist > 0.f) {
                float t = std::min(remaining / nextDist, 1.f);
                m_sprite->setPosition(target + nextDelta * t);
            }
        }
    } else {
        sf::Vector2f dir = delta / dist;
        m_sprite->move(dir * step);
    }
}

// ─── update 
void Enemy::update(float dt) {
    if (m_reached || !m_sprite.has_value()) return;

    m_animTimer += dt;
    if (m_animTimer >= 0.3f) {
        m_animTimer = 0.f;
        m_useFirst  = !m_useFirst;
        m_sprite->setTexture(m_useFirst ? m_tex1 : m_tex2, true);
    }

    moveAlongPath(dt);
}

// ─── drawHealthBar 
void Enemy::drawHealthBar(sf::RenderWindow& window) const {
    if (!m_sprite.has_value()) return;

    sf::FloatRect bounds = m_sprite->getGlobalBounds();

    const float barW   = bounds.size.x;
    const float barH   = 5.f;
    const float margin = 2.f;
    float barX = bounds.position.x;
    float barY = bounds.position.y + bounds.size.y + margin;

    sf::RectangleShape bg({ barW, barH });
    bg.setPosition({ barX, barY });
    bg.setFillColor(sf::Color(60, 60, 60, 200));
    window.draw(bg);

    float ratio = static_cast<float>(m_hp) / static_cast<float>(m_maxhp);
    sf::Color barColor = ratio > 0.5f
        ? sf::Color(80, 200, 80)
        : ratio > 0.25f
            ? sf::Color(220, 180, 0)
            : sf::Color(220, 50, 50);

    sf::RectangleShape fill({ barW * ratio, barH });
    fill.setPosition({ barX, barY });
    fill.setFillColor(barColor);
    window.draw(fill);

    if (s_heartLoaded) {
        float heartSize = bounds.size.y * 0.35f;
        float scale     = heartSize / static_cast<float>(s_heartTex.getSize().y);

        sf::Sprite heart(s_heartTex);
        heart.setScale({ scale, scale });
        heart.setPosition({
            barX - heartSize - 2.f,
            barY + (barH - heartSize) / 2.f
        });
        window.draw(heart);
    }
}

// ─── render 
void Enemy::render(sf::RenderWindow& window) const {
    if (m_reached || !m_sprite.has_value()) return;

    window.draw(*m_sprite);
    drawHealthBar(window);
}