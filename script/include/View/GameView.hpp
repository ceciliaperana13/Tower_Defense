#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include <string>

#include "Map.hpp"
#include "WaveManager.hpp"
#include "CountdownTimer.hpp"
#include "TimerView.hpp"
#include "Button.hpp"
#include "TowerController.hpp"

// Map exacte : 33 * 16 * 2 = 1056 x 22 * 16 * 2 = 704
constexpr unsigned WIN_W  = 1056u;
constexpr float    UI_H   = 128.f;
constexpr unsigned WIN_H  = 704u + static_cast<unsigned>(UI_H);  // 832

constexpr float    MAP_H  = 704.f;   // hauteur carte en pixels fenêtre
constexpr float    MAP_SCALE = 2.f;

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

    // Retourne le type de tour du bouton cliqué ("" sinon)
    std::string getTowerTypeAt(sf::Vector2f logicalPos) const;

    static sf::View makeLetterboxView(unsigned screenW, unsigned screenH);

private:
    sf::RenderWindow& m_window;
    Map&              m_map;
    WaveManager&      m_waveManager;
    CountdownTimer&   m_timer;
    TowerController&  m_towerController;
    TimerView         m_timerView;

    // UI panels
    sf::Texture        m_topPanelTex;
    sf::Texture        m_goldPanelTex;
    sf::Texture        m_heartPanelTex;
    sf::RectangleShape m_topPanel;
    sf::RectangleShape m_goldPanel;
    sf::RectangleShape m_heartPanel;

    // Boutons
    std::vector<Button>      m_towerButtons;
    std::vector<std::string> m_towerTypes;
    std::optional<Button>    m_sellButton;
    std::optional<Button>    m_backButton;

    void buildUI();
    sf::RectangleShape makePanelShape(const sf::Texture& tex,
                                      float px, float py,
                                      float pw, float ph) const;
    void drawMap();
    void drawEnemies();
    void drawUIBar();
    void updateHover(sf::Vector2f mousePos);

    // ── Helpers ──────────────────────────────────────────────────────────────
    // Espace logique UI : 320 x 96  →  WIN_W x UI_H
    // X : partagé sur toute la largeur fenêtre
    // Y : offset par MAP_H  (0 = début de la barre en bas)
    float toWinX(float x) const { return x * (float(WIN_W) / 320.f); }
    float toWinW(float w) const { return w * (float(WIN_W) / 320.f); }
    float toUIY(float y)  const { return MAP_H + y * (UI_H / 96.f); }
    float toUIH(float h)  const { return h  * (UI_H / 96.f); }
    // alias pour makePanelShape
    float toWinY(float y) const { return toUIY(y); }
    float toWinH(float h) const { return toUIH(h); }
};