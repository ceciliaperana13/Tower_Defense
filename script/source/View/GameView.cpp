#include "GameView.hpp"
#include <iostream>

static void loadTex(sf::Texture& tex, const std::string& path) {
    if (!tex.loadFromFile(path))
        std::cerr << "[GameView] Missing texture: " << path << "\n";
}

GameView::GameView(sf::RenderWindow& window, Map& map)
    : m_window(window), m_map(map)
{
    buildUI();
}

sf::RectangleShape GameView::makePanelShape(const sf::Texture& tex,
                                             float uiX, float uiY,
                                             float uiW, float uiH) const {
    sf::RectangleShape shape({ toWinW(uiW), toWinH(uiH) });
    shape.setPosition({ toWinX(uiX), toWinY(uiY) });
    shape.setTexture(&tex);
    shape.setFillColor(sf::Color::White);
    return shape;
}

void GameView::buildUI() {
    loadTex(m_topPanelTex,   "../assets/sprites/buttons/top_panel.png");
    loadTex(m_goldPanelTex,  "../assets/sprites/buttons/gold_pannel.png");
    loadTex(m_heartPanelTex, "../assets/sprites/buttons/heart_pannel.png");

    // Positions are full_ui.png local coords (verified by pixel analysis)
    m_topPanel   = makePanelShape(m_topPanelTex,    0.f,  0.f, 192.f, 32.f);
    m_goldPanel  = makePanelShape(m_goldPanelTex,  192.f, 16.f,  64.f, 32.f);
    m_heartPanel = makePanelShape(m_heartPanelTex, 192.f, 48.f,  64.f, 32.f);

    // Tower buttons (centered position required by Button constructor)
    struct BtnDef { const char* path; float x, y, w, h; };
    constexpr BtnDef defs[] = {
        { "../assets/sprites/buttons/buy_normal_tower_button.png",  0.f, 32.f, 64.f, 64.f },
        { "../assets/sprites/buttons/fire_button.png",             64.f, 32.f, 32.f, 32.f },
        { "../assets/sprites/buttons/ice_button.png",              96.f, 32.f, 32.f, 32.f },
        { "../assets/sprites/buttons/earth_button.png",            64.f, 64.f, 32.f, 32.f },
        { "../assets/sprites/buttons/arcane_button.png",           96.f, 64.f, 32.f, 32.f },
    };

    m_towerButtons.reserve(5);
    for (const auto& d : defs) {
        float cx = toWinX(d.x) + toWinW(d.w) / 2.f;
        float cy = toWinY(d.y) + toWinH(d.h) / 2.f;
        m_towerButtons.emplace_back(d.path, cx, cy, toWinW(d.w), toWinH(d.h),
                                    MenuAction::None);
    }

    // Sell button: full_ui pos (128, 32), size 64x64
    m_sellButton.emplace("../assets/sprites/buttons/sell_tower_button.png",
                         toWinX(128.f) + toWinW(64.f) / 2.f,
                         toWinY(32.f)  + toWinH(64.f) / 2.f,
                         toWinW(64.f), toWinH(64.f),
                         MenuAction::None);

    // Back button: full_ui pos (272, 0), size 128x32
    m_backButton.emplace("../assets/sprites/buttons/back_to_main_menu_button.png",
                         toWinX(272.f) + toWinW(128.f) / 2.f,
                         toWinY(0.f)   + toWinH(32.f)  / 2.f,
                         toWinW(128.f), toWinH(32.f),
                         MenuAction::Exit);
}

// Letterbox view: content fills as much of the screen as possible without distortion
sf::View GameView::makeLetterboxView(unsigned screenW, unsigned screenH) {
    float contentRatio  = static_cast<float>(WIN_W) / static_cast<float>(WIN_H);
    float screenRatio   = static_cast<float>(screenW) / static_cast<float>(screenH);

    float viewportW, viewportH, viewportX, viewportY;

    if (screenRatio > contentRatio) {
        // Screen is wider than content — pillarbox (vertical bands)
        viewportH = 1.f;
        viewportW = contentRatio / screenRatio;
        viewportX = (1.f - viewportW) / 2.f;
        viewportY = 0.f;
    } else {
        // Screen is taller than content — letterbox (horizontal bands)
        viewportW = 1.f;
        viewportH = screenRatio / contentRatio;
        viewportX = 0.f;
        viewportY = (1.f - viewportH) / 2.f;
    }

    sf::View view(sf::FloatRect({ 0.f, 0.f },
                                { static_cast<float>(WIN_W), static_cast<float>(WIN_H) }));
    view.setViewport(sf::FloatRect({ viewportX, viewportY },
                                   { viewportW, viewportH }));
    return view;
}

void GameView::render() {
    drawMap();
    drawUIBar();
}

void GameView::drawMap() {
    m_map.draw(m_window, { 0.f, 0.f }, MAP_SCALE);
}

void GameView::drawUIBar() {
    m_window.draw(m_topPanel);
    m_window.draw(m_goldPanel);
    m_window.draw(m_heartPanel);

    for (const auto& btn : m_towerButtons)
        btn.draw(m_window);

    if (m_sellButton) m_sellButton->draw(m_window);
    if (m_backButton) m_backButton->draw(m_window);
}

void GameView::updateHover(sf::Vector2f mousePos) {
    for (auto& btn : m_towerButtons)
        btn.setHovered(btn.contains(mousePos));

    if (m_sellButton) m_sellButton->setHovered(m_sellButton->contains(mousePos));
    if (m_backButton) m_backButton->setHovered(m_backButton->contains(mousePos));
}

void GameView::updateHoverAt(sf::Vector2f logicalPos) {
    updateHover(logicalPos);
}

MenuAction GameView::handleClickAt(sf::Vector2f logicalPos) {
    for (const auto& btn : m_towerButtons)
        if (btn.contains(logicalPos)) return btn.getAction();

    if (m_sellButton && m_sellButton->contains(logicalPos))
        return m_sellButton->getAction();
    if (m_backButton && m_backButton->contains(logicalPos))
        return m_backButton->getAction();

    return MenuAction::None;
}

// Handles non-mouse events (mouse coords are handled by main via mapPixelToCoords)
MenuAction GameView::handleEvent(const sf::Event& event) {
    return MenuAction::None;
}