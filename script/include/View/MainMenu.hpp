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
    sf::RectangleShape shape;
    sf::Texture        texture;
    MenuAction         action { MenuAction::None };

    bool contains(sf::Vector2f point) const;
    void setHovered(bool hovered);
};

struct WalkingCharacter {
    sf::Texture        frames[2];
    sf::RectangleShape shape;
    int                currentFrame { 0 };
    float              animTimer    { 0.f };
    float              animSpeed    { 0.3f };
    float              speed        { 0.f };
    bool               flipped      { false };
};

class MainMenu {
public:
    explicit MainMenu(sf::RenderWindow& window);
    ~MainMenu() = default;

    MenuAction run();

private:
    void loadAssets();
    void buildButtons();
    void loadCharacters();

    void handleEvents(MenuAction& result);
    void update(sf::Vector2f mousePos, float dt);
    void updateCharacters(float dt);
    void render();
    void renderCharacters();

    MenuButton makeButton(const std::string& texturePath,
                          float cx, float cy,
                          float width, float height,
                          MenuAction action);

    sf::RenderWindow& m_window;

    sf::Texture               m_bgTexture;
    std::optional<sf::Sprite> m_bgSprite;

    sf::Texture               m_settingsTexture;
    std::optional<sf::Sprite> m_settingsSprite;

    sf::Font                  m_font;
    std::optional<sf::Text>   m_title;

    std::vector<MenuButton>       m_buttons;
    std::vector<WalkingCharacter> m_characters;

    std::unique_ptr<sf::Music>    m_music;

    sf::Clock m_clock;
    bool      m_running { true };
};