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
#include "WaveView.hpp"
// Map 
constexpr unsigned WIN_W  = 1056u;
constexpr float    UI_H   = 128.f;
constexpr unsigned WIN_H  = static_cast<unsigned>(704.f + UI_H);  // 832

constexpr float    MAP_H  = 704.f;
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

    
    // En mode upgrade, retourne le type lv2 correspondant
    std::string getTowerTypeAt(sf::Vector2f logicalPos) const;

    // True si le clic est dans la zone carte (pas UI)
    bool isMapClick(sf::Vector2f pos) const { return pos.y < MAP_H; }

    static sf::View makeLetterboxView(unsigned screenW, unsigned screenH);

    // Coins & vies affichés
    void setLives(int lives) { m_lives = lives; }

private:
    sf::RenderWindow& m_window;
    Map&              m_map;
    WaveManager&      m_waveManager;
    CountdownTimer&   m_timer;
    TowerController&  m_towerController;
    TimerView         m_timerView;
    WaveView          m_waveView;
    // ── Panels fond UI 
    sf::Texture        m_topPanelTex;
    sf::Texture        m_goldPanelTex;
    sf::Texture        m_heartPanelTex;
    sf::RectangleShape m_topPanel;
    sf::RectangleShape m_goldPanel;
    sf::RectangleShape m_heartPanel;

    // ── Boutons tours (placement + upgrade) 
    std::vector<Button>      m_towerButtons;   // 5 boutons : basic + 4 lv2
    std::vector<std::string> m_towerTypes;     // type associé à chaque bouton

    std::optional<Button> m_sellButton;
    std::optional<Button> m_backButton;

    // ── Texte coins / vies 
    sf::Font m_font;
    sf::Text m_coinsText;
    sf::Text m_livesText;

    int m_lives = 20;

    // ── Surbrillance tour sélectionnée pour upgrade ──────────────────────────
    sf::CircleShape m_upgradeHighlight;

    void buildUI();
    sf::RectangleShape makePanelShape(const sf::Texture& tex,
                                      float px, float py,
                                      float pw, float ph) const;
    void drawMap();
    void drawEnemies();
    void drawUIBar();
    void drawUpgradeHighlight();
    void updateHover(sf::Vector2f mousePos);
    void updateTexts();

    // Helpers : espace logique UI 320x96 → WIN_W x UI_H
    float toWinX(float x) const { return x * (float(WIN_W) / 320.f); }
    float toWinW(float w) const { return w * (float(WIN_W) / 320.f); }
    float toUIY(float y)  const { return MAP_H + y * (UI_H / 96.f); }
    float toUIH(float h)  const { return h * (UI_H / 96.f); }
    float toWinY(float y) const { return toUIY(y); }
    float toWinH(float h) const { return toUIH(h); }
};