#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <optional>
#include <memory>
#include <string>


enum class MenuAction {
    NewGame,
    Scoreboard,
    Settings,
    Exit,
    None
};

struct MenuButton {
    sf::RectangleShape        shape;
    sf::Texture               texture;
    sf::Texture               hoverTexture;   // même texture, teinte différente
    MenuAction                action { MenuAction::None };

    bool contains(sf::Vector2f point) const;
    void setHovered(bool hovered);
};


class MainMenu {
public:
    explicit MainMenu(sf::RenderWindow& window);
    ~MainMenu() = default;

    MenuAction run();

private:
    void loadAssets();
    void buildButtons();

    void handleEvents(MenuAction& result);
    void update(sf::Vector2f mousePos);
    void render();

    MenuButton makeButton(const std::string& texturePath,
                          float cx, float cy,
                          float width, float height,
                          MenuAction action);

    sf::RenderWindow& m_window;

    // Background
    sf::Texture                m_bgTexture;
    std::optional<sf::Sprite>  m_bgSprite;

    // Settings icon (coin haut-gauche)
    sf::Texture                m_settingsTexture;
    std::optional<sf::Sprite>  m_settingsSprite;

    // Titre
    sf::Font                   m_font;
    std::optional<sf::Text>    m_title;

    // Boutons
    std::vector<MenuButton>    m_buttons;

    // Musique
    std::unique_ptr<sf::Music> m_music;

    bool                       m_running { true };
};