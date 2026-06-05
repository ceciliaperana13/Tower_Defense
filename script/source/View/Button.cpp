#include "View/Button.hpp"
#include <iostream>

// ─── Constructeur 
Button::Button(const std::string& texturePath,
               float cx, float cy,
               float width, float height,
               MenuAction action)
    : m_action(action)
{
    if (!m_texture.loadFromFile(texturePath))
        std::cerr << "[Button] Texture introuvable : " << texturePath << "\n";

    m_shape.setSize({ width, height });
    m_shape.setOrigin({ width / 2.f, height / 2.f });
    m_shape.setPosition({ cx, cy });
    m_shape.setFillColor(sf::Color::White);
    m_shape.setTexture(&m_texture);
}

// ─── contains 
bool Button::contains(sf::Vector2f point) const {
    return m_shape.getGlobalBounds().contains(point);
}

// ─── setHovered 
void Button::setHovered(bool hovered) {
    m_shape.setFillColor(hovered ? sf::Color(180, 180, 180, 255)
                                 : sf::Color(255, 255, 255, 255));
}

// ─── draw 
void Button::draw(sf::RenderWindow& window) const {
    window.draw(m_shape);
}