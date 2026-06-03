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

    // Charge le sprite de cœur partagé (à appeler une fois avant le jeu)
    static bool loadHeartTexture(const std::string& path);

    void update(float dt);
    void render(sf::RenderWindow& window) const;

    void takeDamage(int dmg);
    void applySpeedMultiplier(float multiplier) { m_speed = m_maxspeed * multiplier; }

    bool isDead()     const { return m_hp <= 0; }
    bool hasReached() const { return m_reached; }
    int  getId()      const { return m_id; }
    int  getReward()  const { return m_reward; }

    // 🔥 AJOUTS OBLIGATOIRES POUR SFML 3 + TowerController
    sf::Vector2f getPosition() const {
        if (m_sprite.has_value())
            return m_sprite->getPosition();
        return sf::Vector2f(0.f, 0.f);
    }

    sf::FloatRect getBounds() const {
        if (m_sprite.has_value())
            return m_sprite->getGlobalBounds();
        return sf::FloatRect();
    }

private:
    void moveAlongPath(float dt);
    void drawHealthBar(sf::RenderWindow& window) const;

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
    int                       m_waypointIdx { 1 };
    bool                      m_reached     { false };

    // Texture partagée entre tous les ennemis (static)
    static sf::Texture s_heartTex;
    static bool        s_heartLoaded;
};
