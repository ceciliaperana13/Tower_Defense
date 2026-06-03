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

class GameView {
public:
    GameView(sf::RenderWindow& window,
             Map& map,
             WaveManager& waveManager,
             CountdownTimer& timer);

    void update(float dt);
    void render();

    void updateHover(sf::Vector2f mousePos);
    void updateHoverAt(sf::Vector2f logicalPos);

    MenuAction handleEvent(const sf::Event& event);
    MenuAction handleClickAt(sf::Vector2f logicalPos);

private:
    sf::RenderWindow& m_window;
    Map&              m_map;
    WaveManager&      m_waveManager;
    CountdownTimer&   m_countdownTimer;

    TimerView         m_timerView;

    // UI panels
    sf::Texture m_topPanelTex;
    sf::Texture m_goldPanelTex;
    sf::Texture m_heartPanelTex;

    sf::RectangleShape m_topPanel;
    sf::RectangleShape m_goldPanel;
    sf::RectangleShape m_heartPanel;

    // Tower buttons
    std::vector<Button> m_towerButtons;
    std::optional<Button> m_sellButton;
    std::optional<Button> m_backButton;

    // Tower controller (lié à GameView)
    TowerController m_towerController;

private:
    void buildUI();
    sf::RectangleShape makePanelShape(const sf::Texture& tex,
                                      float uiX, float uiY,
                                      float uiW, float uiH) const;

    void drawMap();
    void drawEnemies();
    void drawUIBar();

    static sf::View makeLetterboxView(unsigned screenW, unsigned screenH);
};
