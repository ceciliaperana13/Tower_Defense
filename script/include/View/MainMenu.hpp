#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <functional>
#include <optional>


enum class MenuAction {
    NewGame,
    Scoreboard,
    Settings,
    Exit,
    None
};

// Composant bouton interne
struct MenuButton {
    sf::RectangleShape        shape;
    std::optional<sf::Text>   label;   // SFML 3 : pas de constructeur par défaut
    sf::Color                 normalColor;
    sf::Color                 hoverColor;
    MenuAction                action { MenuAction::None };

    bool contains(sf::Vector2f point) const;
    void setHovered(bool hovered);
};


// Classe principale du menu
class MainMenu {
public:
    explicit MainMenu(sf::RenderWindow& window);
    ~MainMenu() = default;

    MenuAction run();

private:
    void loadAssets();
    void buildButtons();
    void buildSettingsIcon();

    void handleEvents(MenuAction& result);
    void update(sf::Vector2f mousePos);
    void render();

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
    sf::Texture                m_bgTexture;
    std::optional<sf::Sprite>  m_bgSprite;

    // Titre — SFML 3 : sf::Text exige une font à la construction
    sf::Font                   m_font;
    std::optional<sf::Text>    m_title;

    // Boutons
    std::vector<MenuButton>    m_buttons;

    // Icône settings
    sf::RectangleShape         m_settingsIcon;
    sf::Texture                m_settingsTexture;
    std::optional<sf::Sprite>  m_settingsSprite;
    bool                       m_hasSettingsTexture { false };

    bool                       m_running { true };
};