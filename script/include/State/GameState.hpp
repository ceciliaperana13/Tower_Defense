#pragma once
#include "State/IState.hpp"
#include "Model/Map.hpp"
#include "Model/WaveManager.hpp"
#include "Model/CountdownTimer.hpp"
#include "Model/Castle.hpp"
#include "Controller/TowerController.hpp"
#include "Controller/SaveController.hpp"
#include "View/GameView.hpp"

class GameState : public IState {
public:
    explicit GameState(sf::RenderWindow& window, bool demoMode = false);

    void onEnter() override;
    void onExit()  override;

    void handleEvents(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void handleMouseClick(sf::Vector2f wp);
    void checkEndConditions();
    void flushScore();
    void goBackToMenu();

    sf::RenderWindow& m_window;

    bool m_demoMode { false }; // must be first: used during m_waveManager init

    Map             m_map;
    WaveManager     m_waveManager;
    CountdownTimer  m_timer;
    Castle          m_castle;

    TowerController m_towerController;

    GameView        m_gameView;

    sf::CircleShape m_upgradeRing;
    bool            m_showUpgradeRing { false };
    sf::Vector2f    m_upgradeRingPos;

    int  m_prevReached { 0 };
    bool m_ended       { false };
};