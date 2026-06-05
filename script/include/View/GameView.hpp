#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include <string>
 
#include "Model/Map.hpp"
#include "Model/WaveManager.hpp"
#include "Model/CountdownTimer.hpp"
#include "View/TimerView.hpp"
#include "View/Button.hpp"
#include "Controller/TowerController.hpp"
#include "View/WaveView.hpp"
 
// ── Constantes fenêtre/map 
constexpr unsigned WIN_W     = 1056u;
constexpr float    UI_H      = 128.f;
constexpr unsigned WIN_H     = static_cast<unsigned>(704.f + UI_H);
constexpr float    MAP_H     = 704.f;
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
 
    void       updateHoverAt(sf::Vector2f logicalPos);
    MenuAction handleClickAt(sf::Vector2f logicalPos);
    MenuAction handleEvent(const sf::Event& event);
 
    std::string getTowerTypeAt(sf::Vector2f logicalPos) const;
    bool        isMapClick(sf::Vector2f pos) const { return pos.y < MAP_H; }
 
    static sf::View makeLetterboxView(unsigned screenW, unsigned screenH);
 
    void setLives(int lives) { m_lives = lives; }
 
    // ── Popup confirmation 
    bool hasConfirmPending()  const { return m_confirmPending; }
    void showConfirm(const std::string& towerType, bool isUpgrade, sf::Vector2f mapPos);
    void handleConfirmClick(sf::Vector2f pos);
    void cancelConfirm();
 
private:
    sf::RenderWindow& m_window;
    Map&              m_map;
    WaveManager&      m_waveManager;
    CountdownTimer&   m_timer;
    TowerController&  m_towerController;
    TimerView         m_timerView;
    WaveView          m_waveView;
 
    sf::Texture        m_topPanelTex;
    sf::Texture        m_goldPanelTex;
    sf::Texture        m_heartPanelTex;
    sf::RectangleShape m_topPanel;
    sf::RectangleShape m_goldPanel;
    sf::RectangleShape m_heartPanel;
 
    std::vector<Button>      m_towerButtons;
    std::vector<std::string> m_towerTypes;
    std::optional<Button>    m_sellButton;
    std::optional<Button>    m_backButton;
 
    std::vector<sf::Text> m_priceTags;
 
    sf::Font m_font;
    sf::Text m_coinsText;
    sf::Text m_livesText;
    int      m_lives { 20 };
 
    sf::CircleShape m_upgradeHighlight;
 
    // ── Popup au-dessus de l'UI 
    bool         m_confirmPending   { false };
    bool         m_confirmIsUpgrade { false };
    std::string  m_confirmType;
    sf::Vector2f m_confirmMapPos;
 
    sf::RectangleShape      m_confirmPanel;
    std::optional<sf::Text> m_confirmTitle;
    std::optional<sf::Text> m_confirmPrice;
    sf::RectangleShape      m_confirmYesBtn;
    sf::RectangleShape      m_confirmNoBtn;
    std::optional<sf::Text> m_confirmYesLabel;
    std::optional<sf::Text> m_confirmNoLabel;
    bool m_confirmYesHover { false };
    bool m_confirmNoHover  { false };
 
    void buildUI();
    void buildConfirmPopup();
    sf::RectangleShape makePanelShape(const sf::Texture& tex,
                                      float px, float py,
                                      float pw, float ph) const;
    void drawMap();
    void drawEnemies();
    void drawUIBar();
    void drawPriceTags();
    void drawUpgradeHighlight();
    void drawConfirmPopup();
    void updateHover(sf::Vector2f mousePos);
    void updateTexts();
    void centerText(sf::Text& text, const sf::RectangleShape& btn) const;
 
    float toWinX(float x) const { return x * (float(WIN_W) / 320.f); }
    float toWinW(float w) const { return w * (float(WIN_W) / 320.f); }
    float toUIY(float y)  const { return MAP_H + y * (UI_H / 96.f); }
    float toUIH(float h)  const { return h * (UI_H / 96.f); }
};