#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>

class Enemy {
public:
    Enemy(int id,
          int maxhp,
          float maxspeed,
          int reward,
          const std::string& sprite1Path,
          const std::string& sprite2Path,
          const std::vector<sf::Vector2f>& waypoints);

    static Enemy fromJson(const std::string& jsonPath,
                          const std::string& type,
                          const std::vector<sf::Vector2f>& waypoints);

    void update(float dt);
    void render(sf::RenderWindow& window) const;

    void applySpeedMultiplier(float multiplier) { m_speed = m_maxspeed * multiplier; }

    bool  isDead()       const { return m_hp <= 0; }
    bool  hasReached()   const { return m_reached; }  // reached the castle
    int   getId()        const { return m_id; }
    int   getReward()    const { return m_reward; }

private:
    void moveAlongPath(float dt);

    int   m_id;
    int   m_maxhp;
    int   m_hp;
    float m_maxspeed;
    float m_speed;
    int   m_reward;

    sf::Texture               m_tex1;
    sf::Texture               m_tex2;
    std::optional<sf::Sprite> m_sprite;

    float m_animTimer { 0.f };
    bool  m_useFirst  { true };

    std::vector<sf::Vector2f> m_waypoints;
    int                       m_waypointIdx { 1 };  // heading toward this index
    bool                      m_reached     { false };
};