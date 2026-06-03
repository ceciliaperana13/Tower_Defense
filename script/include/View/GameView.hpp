#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>

#include "Map.hpp"
#include "WaveManager.hpp"
#include "CountdownTimer.hpp"
#include "TimerView.hpp"
#include "Button.hpp"
#include "TowerController.hpp"

constexpr unsigned WIN_W = 960;
constexpr unsigned WIN_H = 540;

constexpr float MAP_SCALE = 2.f;

class GameView {
public:
    GameView(sf::RenderWindow& window,
             Map& map,
             WaveManager& waveManager,
             CountdownTimer& timer,
             TowerController& towerController);

    void update(float dt);
    void render();

    void updateHoverAt(sf::Vector2f logicalPos);
    MenuAction handleClickAt(sf::Vector2f logicalPos);
    MenuAction handleEvent(const sf::Event& event);

    static sf::View makeLetterboxView(unsigned screenW, unsigned screenH);

private:
    sf::RenderWindow& m_window;
    Map&              m_map;
    WaveManager&      m_waveManager;
    CountdownTimer&   m_timer;
    TowerController&  m_towerController;

    TimerView         m_timerView;

    // UI panels
    sf::Texture m_topPanelTex;
    sf::Texture m_goldPanelTex;
    sf::Texture m_heartPanelTex;

    sf::RectangleShape m_topPanel;
    sf::RectangleShape m_goldPanel;
    sf::RectangleShape m_heartPanel;

    // Buttons
    std::vector<Button> m_towerButtons;
    std::optional<Button> m_sellButton;
    std::optional<Button> m_backButton;

private:
    void buildUI();
    sf::RectangleShape makePanelShape(const sf::Texture& tex,
                                      float uiX, float uiY,
                                      float uiW, float uiH) const;

    void drawMap();
    void drawEnemies();
    void drawUIBar();

    void updateHover(sf::Vector2f mousePos);

    // Conversion helpers
    float toWinX(float x) const { return x * (WIN_W / 320.f); }
    float toWinY(float y) const { return y * (WIN_H / 180.f); }
    float toWinW(float w) const { return w * (WIN_W / 320.f); }
    float toWinH(float h) const { return h * (WIN_H / 180.f); }
};
