#include "Castle.hpp"
#include <iostream>
#include <algorithm>

// ─── Constructeur ─────────────────────────────────────────────────────────────
Castle::Castle(const std::string& spritePath,
               sf::Vector2f       position,
               int                maxLives)
    : m_sprite(m_texture)
    , m_maxLives(maxLives)
    , m_lives(maxLives)
{
    if (!m_texture.loadFromFile(spritePath))
        std::cerr << "[Castle] Sprite introuvable : " << spritePath << "\n";

    m_sprite.setTexture(m_texture, true);
    m_sprite.setScale({ 2.f, 2.f });

    // Origine au centre-bas du sprite pour l'ancrer sur le waypoint
    sf::FloatRect b = m_sprite.getLocalBounds();
    m_sprite.setOrigin({ b.size.x / 2.f, b.size.y });
    m_sprite.setPosition(position);
}

// ─── loseLife ─────────────────────────────────────────────────────────────────
void Castle::loseLife(int amount) {
    m_lives = std::max(0, m_lives - amount);
}

// ─── reset ────────────────────────────────────────────────────────────────────
void Castle::reset() {
    m_lives = m_maxLives;
}

// ─── drawHealthBar ────────────────────────────────────────────────────────────
void Castle::drawHealthBar(sf::RenderWindow& window) const {
    sf::FloatRect bounds = m_sprite.getGlobalBounds();

    const float barW   = bounds.size.x;
    const float barH   = 8.f;
    const float margin = 4.f;
    float barX = bounds.position.x;
    float barY = bounds.position.y - barH - margin;  // au-dessus du sprite

    // Fond sombre
    sf::RectangleShape bg({ barW, barH });
    bg.setPosition({ barX, barY });
    bg.setFillColor(sf::Color(40, 40, 40, 220));
    bg.setOutlineColor(sf::Color(0, 0, 0, 180));
    bg.setOutlineThickness(1.f);
    window.draw(bg);

    // Remplissage coloré selon % de vie
    float ratio = static_cast<float>(m_lives) / static_cast<float>(m_maxLives);
    sf::Color barColor =
        ratio > 0.5f  ? sf::Color(60,  200, 60)   :   // vert
        ratio > 0.25f ? sf::Color(220, 180,  0)   :   // jaune
                        sf::Color(220,  50, 50);       // rouge

    sf::RectangleShape fill({ barW * ratio, barH });
    fill.setPosition({ barX, barY });
    fill.setFillColor(barColor);
    window.draw(fill);

    // Petits segments (un par vie) pour visualiser chaque point de vie
    if (m_maxLives <= 30) {
        float segW = barW / static_cast<float>(m_maxLives);
        for (int i = 1; i < m_maxLives; ++i) {
            sf::RectangleShape sep({ 1.f, barH });
            sep.setPosition({ barX + segW * i, barY });
            sep.setFillColor(sf::Color(0, 0, 0, 120));
            window.draw(sep);
        }
    }
}

// ─── render ───────────────────────────────────────────────────────────────────
void Castle::render(sf::RenderWindow& window) const {
    window.draw(m_sprite);
    drawHealthBar(window);
}