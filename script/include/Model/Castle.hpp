#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Castle {
public:
    Castle(const std::string& spritePath,
           sf::Vector2f        position,
           int                 maxLives = 20);

    void render(sf::RenderWindow& window) const;

    void loseLife(int amount = 1);
    void reset();

    int  getLives()    const { return m_lives; }
    int  getMaxLives() const { return m_maxLives; }
    bool isDead()      const { return m_lives <= 0; }

    sf::Vector2f getPosition() const { return m_sprite.getPosition(); }

private:
    void drawHealthBar(sf::RenderWindow& window) const;

    sf::Texture  m_texture;
    sf::Sprite   m_sprite;

    int m_maxLives;
    int m_lives;
};