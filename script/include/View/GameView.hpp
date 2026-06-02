#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include "Map.hpp"
#include "Button.hpp"
#include "TimerView.hpp"
#include "CountdownTimer.hpp"

class GameView {
public:
    static constexpr unsigned WIN_W = 1056u;
    static constexpr unsigned WIN_H = 957u;

    GameView(sf::RenderWindow& window, Map& map, CountdownTimer& timer);

    void update(float dt);
    void render();

    void       updateHoverAt(sf::Vector2f logicalPos);
    MenuAction handleClickAt(sf::Vector2f logicalPos);
    MenuAction handleEvent(const sf::Event& event);

    static sf::View makeLetterboxView(unsigned screenW, unsigned screenH);

private:
    static constexpr float MAP_SCALE = 2.f;
    static constexpr float MAP_H     = Map::ROWS * Map::TILE_SIZE * MAP_SCALE;
    static constexpr float UI_SCALE  = static_cast<float>(WIN_W) / 400.f;

    static float toWinX(float x) { return x * UI_SCALE; }
    static float toWinY(float y) { return MAP_H + y * UI_SCALE; }
    static float toWinW(float w) { return w * UI_SCALE; }
    static float toWinH(float h) { return h * UI_SCALE; }

    void buildUI();
    void drawMap();
    void drawUIBar();
    void updateHover(sf::Vector2f mousePos);

    sf::RectangleShape makePanelShape(const sf::Texture& tex,
                                      float uiX, float uiY,
                                      float uiW, float uiH) const;

    sf::RenderWindow& m_window;
    Map&              m_map;

    // Panels
    sf::Texture        m_topPanelTex;
    sf::Texture        m_goldPanelTex;
    sf::Texture        m_heartPanelTex;
    sf::RectangleShape m_topPanel;
    sf::RectangleShape m_goldPanel;
    sf::RectangleShape m_heartPanel;

    // Buttons
    std::vector<Button>   m_towerButtons;
    std::optional<Button> m_sellButton;
    std::optional<Button> m_backButton;

    // Timer — affiché dans le top panel, centré horizontalement
    CountdownTimer& m_countdownTimer;
    TimerView       m_timerView;
};