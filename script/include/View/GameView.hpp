#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include "Map.hpp"
#include "Button.hpp"
#include "WaveManager.hpp"
#include "TimerView.hpp"
#include "CountdownTimer.hpp"
#include "TowerController.hpp"

class GameView {
public:
    static constexpr unsigned WIN_W = 1056u;
    static constexpr unsigned WIN_H = 957u;

    GameView(sf::RenderWindow&  window,
             Map&               map,
             WaveManager&       waveManager,
             CountdownTimer&    timer,
             TowerController&   towerController);

    void update(float dt);
    void render();

    void       updateHoverAt (sf::Vector2f logicalPos);
    MenuAction handleClickAt (sf::Vector2f logicalPos);
    MenuAction handleEvent   (const sf::Event& event);

    static sf::View makeLetterboxView(unsigned screenW, unsigned screenH);

private:
    // ── Constantes de mise en page ──────────────────────────────────────
    static constexpr float MAP_SCALE = 2.f;
    static constexpr float MAP_H     = Map::ROWS * Map::TILE_SIZE * MAP_SCALE;
    static constexpr float UI_SCALE  = static_cast<float>(WIN_W) / 400.f;

    // Noms de tours dans le même ordre que m_towerButtons (defs dans buildUI)
    static constexpr const char* TOWER_NAMES[] = {
        "basic", "fire", "ice", "earth", "arcane"
    };

    // ── Helpers coordonnées UI 
    static float toWinX(float x) { return x * UI_SCALE; }
    static float toWinY(float y) { return MAP_H + y * UI_SCALE; }
    static float toWinW(float w) { return w * UI_SCALE; }
    static float toWinH(float h) { return h * UI_SCALE; }

    // ── Méthodes internes
    void buildUI();
    void drawMap();
    void drawEnemies();
    void drawTowers();
    void drawUIBar();
    void updateHover(sf::Vector2f mousePos);
    bool isOverUI(sf::Vector2f logicalPos) const;

    sf::RectangleShape makePanelShape(const sf::Texture& tex,
                                      float uiX, float uiY,
                                      float uiW, float uiH) const;

    // ── Références externes 
    sf::RenderWindow& m_window;
    Map&              m_map;
    WaveManager&      m_waveManager;
    CountdownTimer&   m_countdownTimer;
    TowerController&  m_towerController;

    // ── UI panels 
    sf::Texture        m_topPanelTex;
    sf::Texture        m_goldPanelTex;
    sf::Texture        m_heartPanelTex;
    sf::RectangleShape m_topPanel;
    sf::RectangleShape m_goldPanel;
    sf::RectangleShape m_heartPanel;

    std::vector<Button>   m_towerButtons;
    std::optional<Button> m_sellButton;
    std::optional<Button> m_backButton;

    TimerView    m_timerView;

    // ── État souris (position logique courante) 
    sf::Vector2f m_mousePos;

    // ── Rectangle de la zone UI (pour exclure le ghost) 
    sf::FloatRect m_uiRect;
};