#pragma once

#include <SFML/Graphics.hpp>
#include <string>

enum class MenuAction {
    None,

    NewGame,
    Scoreboard,
    Settings,
    Exit,

    BasicTower,
    SellTower
};

class Button {
public:
    Button(const std::string& texturePath,
           float cx, float cy,
           float width, float height,
           MenuAction action);

    bool contains(sf::Vector2f point) const;
    void setHovered(bool hovered);
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible()        const { return m_visible; }

    MenuAction    getAction() const { return m_action; }
    sf::FloatRect getBounds() const { return m_shape.getGlobalBounds(); }

    void draw(sf::RenderWindow& window) const;

private:
    sf::Texture        m_texture;
    sf::RectangleShape m_shape;
    MenuAction         m_action  { MenuAction::None };
    bool               m_visible { true };
};