#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include "Map.hpp"
#include "Button.hpp"

// Map occupies the top portion, UI bar the bottom.
// All dimensions are derived from the map natural size at integer scale.
class GameView {
public:
    // Window dimensions — sized to fit map + UI with no cropping
    static constexpr unsigned WIN_W = 1056u;  // 528 * 2
    static constexpr unsigned WIN_H = 957u;   // 704 + 253

    GameView(sf::RenderWindow& window, Map& map);

    void update(float dt) {}
    void render();

    // Called from main with already-converted logical coordinates
    void       updateHoverAt(sf::Vector2f logicalPos);
    MenuAction handleClickAt(sf::Vector2f logicalPos);

    // Handles non-mouse events only
    MenuAction handleEvent(const sf::Event& event);

    // Returns a letterboxed view for fullscreen rendering (no distortion)
    static sf::View makeLetterboxView(unsigned screenW, unsigned screenH);

private:
    static constexpr float MAP_SCALE = 2.f;
    static constexpr float MAP_H     = Map::ROWS * Map::TILE_SIZE * MAP_SCALE; // 704
    static constexpr float UI_SCALE  = static_cast<float>(WIN_W) / 400.f;     // 2.64

    // Converts full_ui-local coords to window coords
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

    // Static panels (non-interactive)
    sf::Texture        m_topPanelTex;
    sf::Texture        m_goldPanelTex;
    sf::Texture        m_heartPanelTex;
    sf::RectangleShape m_topPanel;
    sf::RectangleShape m_goldPanel;
    sf::RectangleShape m_heartPanel;

    // Interactive buttons — using std::optional to avoid dummy construction
    std::vector<Button>  m_towerButtons;
    std::optional<Button> m_sellButton;
    std::optional<Button> m_backButton;
};