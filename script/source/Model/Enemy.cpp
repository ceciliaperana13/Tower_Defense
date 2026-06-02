#include "Enemy.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cmath>

using json = nlohmann::json;

Enemy::Enemy(int id, int maxhp, float maxspeed, int reward,
             const std::string& sprite1Path,
             const std::string& sprite2Path,
             const std::vector<sf::Vector2f>& waypoints)
    : m_id(id), m_maxhp(maxhp), m_hp(maxhp),
      m_maxspeed(maxspeed), m_speed(maxspeed),
      m_reward(reward),
      m_waypoints(waypoints)
{
    if (!m_tex1.loadFromFile(sprite1Path))
        std::cerr << "[Enemy] Introuvable : " << sprite1Path << "\n";
    if (!m_tex2.loadFromFile(sprite2Path))
        std::cerr << "[Enemy] Introuvable : " << sprite2Path << "\n";

    m_sprite.emplace(m_tex1);
    m_sprite->setScale(sf::Vector2f(2.f, 2.f));

    if (!m_waypoints.empty())
        m_sprite->setPosition(m_waypoints[0]);
}

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

void Enemy::moveAlongPath(float dt) {
    if (m_reached || m_waypointIdx >= (int)m_waypoints.size())
        return;

    sf::Vector2f pos    = m_sprite->getPosition();
    sf::Vector2f target = m_waypoints[m_waypointIdx];
    sf::Vector2f delta  = target - pos;

    float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    float step = m_speed * dt;

    if (step >= dist) {
        // Reached this waypoint, advance to the next
        m_sprite->setPosition(target);
        ++m_waypointIdx;

        if (m_waypointIdx >= (int)m_waypoints.size()) {
            m_reached = true;  // reached the castle
        } else {
            // Continue with remaining movement this frame
            float remaining = step - dist;
            sf::Vector2f next = m_waypoints[m_waypointIdx];
            sf::Vector2f nextDelta = next - target;
            float nextDist = std::sqrt(nextDelta.x*nextDelta.x + nextDelta.y*nextDelta.y);
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

void Enemy::update(float dt) {
    if (m_reached) return;

    // Animate sprite
    m_animTimer += dt;
    if (m_animTimer >= 0.3f) {
        m_animTimer = 0.f;
        m_useFirst  = !m_useFirst;
        m_sprite->setTexture(m_useFirst ? m_tex1 : m_tex2);
    }

    moveAlongPath(dt);
}

void Enemy::render(sf::RenderWindow& window) const {
    if (!m_reached && m_sprite.has_value())
        window.draw(*m_sprite);
}