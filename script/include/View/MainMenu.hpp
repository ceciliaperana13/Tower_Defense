#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <functional>


enum class MenuAction {
    NewGame,
    Scoreboard,
    Settings,
    Exit,
    None
};

// Composant bouton interne
struct MenuButton {
    sf::RectangleShape shape;
    sf::Text           label;
    sf::Color          normalColor;
    sf::Color          hoverColor;
    MenuAction         action;

    bool contains(sf::Vector2f point) const;
    void setHovered(bool hovered);
};


// Classe principale du menu
class MainMenu {
public:
    // Constructeur : passe la fenêtre par référence
    explicit MainMenu(sf::RenderWindow& window);
    ~MainMenu() = default;

    // Boucle principale du menu — retourne l'action choisie
    MenuAction run();

private:
    // ── Initialisation 
    void loadAssets();
    void buildButtons();
    void buildSettingsIcon();

    // ── Boucle interne 
    void handleEvents(MenuAction& result);
    void update(sf::Vector2f mousePos);
    void render();

    // ── Utilitaires 
    MenuButton makeButton(const std::string& text,
                          float              cx,
                          float              cy,
                          float              width,
                          float              height,
                          MenuAction         action,
                          sf::Color          normal,
                          sf::Color          hover);

    // ── Références / ressources
    sf::RenderWindow& m_window;

    // Background
    sf::Texture        m_bgTexture;
    sf::Sprite         m_bgSprite;

    // Titre
    sf::Font           m_font;
    sf::Text           m_title;

    // Boutons du menu
    std::vector<MenuButton> m_buttons;

    // Icône settings (engrenage, coin haut-gauche)
    sf::RectangleShape m_settingsIcon;
    sf::Texture        m_settingsTexture;
    sf::Sprite         m_settingsSprite;
    bool               m_hasSettingsTexture { false };

    // État
    bool               m_running { true };
};