#pragma once

#include <SFML/Graphics.hpp>
#include <string>

enum class MenuAction {
    NewGame,
    Scoreboard,
    Settings,
    Exit,
    None,
    BasicTower
};

class Button {
public:
    // Construit un bouton centré en (cx, cy) avec la texture donnée
    Button(const std::string& texturePath,
           float cx, float cy,
           float width, float height,
           MenuAction action);

    // Retourne vrai si le point est dans le bouton
    bool contains(sf::Vector2f point) const;

    // Change l'apparence selon le survol
    void setHovered(bool hovered);

    // Retourne l'action associée
    MenuAction getAction() const { return m_action; }

    // Dessine le bouton dans la fenêtre
    void draw(sf::RenderWindow& window) const;

private:
    sf::Texture        m_texture;
    sf::RectangleShape m_shape;
    MenuAction         m_action { MenuAction::None };
};