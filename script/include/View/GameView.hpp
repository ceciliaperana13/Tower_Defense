#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

#include "Map.hpp"
#include "WaveManager.hpp"
#include "CountdownTimer.hpp"
#include "TimerView.hpp"
#include "Button.hpp"
#include "TowerController.hpp"

// Dimensions logiques
constexpr unsigned WIN_W = 960;
constexpr unsigned WIN_H = 540;

// Taille réelle de la map Tiled : 22 tiles * 16px = 352px
constexpr float MAP_H = 352.f;

// Échelle de la map
constexpr float MAP_SCALE = 2.f;

// Noms des tours (ordre = boutons)
static constexpr const char* TOWER_NAMES[] = {
    "basic",
    "fire",
    "ice",
    "rock",
    "arcane"
};

class GameView {
public:
    GameView(sf::RenderWindow& window,
             Map& map,
             WaveManager& waveManager,
             CountdownTimer& timer,
             TowerController& towerController);

    void update(float dt);
    void render();

    void updateHover(sf::Vector2f mousePos);
    void updateHoverAt(sf::Vector2f logicalPos);

    MenuAction handleEvent(const sf::Event& event);
    MenuAction handleClickAt(sf::Vector2f logicalPos);

    static sf::View makeLetterboxView(unsigned screenW, unsigned screenH);

private:
    sf::RenderWindow& m_window;
    Map&              m_map;
    WaveManager&      m_waveManager;
    CountdownTimer&   m_countdownTimer;

    TimerView         m_timerView;

    sf::Texture m_topPanelTex;
    sf::Texture m_goldPanelTex;
    sf::Texture m_heartPanelTex;

    sf::RectangleShape m_topPanel;
    sf::RectangleShape m_goldPanel;
    sf::RectangleShape m_heartPanel;

    std::vector<Button> m_towerButtons;
    std::optional<Button> m_sellButton;
    std::optional<Button> m_backButton;

    TowerController& m_towerController;

    sf::FloatRect m_uiRect;
    sf::Vector2f m_mousePos;

private:
    void buildUI();
    sf::RectangleShape makePanelShape(const sf::Texture& tex,
                                      float uiX, float uiY,
                                      float uiW, float uiH) const;

    void drawMap();
    void drawEnemies();
    void drawTowers();
    void drawUIBar();

    bool isOverUI(sf::Vector2f logicalPos) const;
};
