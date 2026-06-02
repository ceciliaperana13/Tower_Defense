#include "GameView.hpp"
#include <iostream>

static void loadTex(sf::Texture& tex, const std::string& path) {
    if (!tex.loadFromFile(path))
        std::cerr << "[GameView] Missing texture: " << path << "\n";
}

GameView::GameView(sf::RenderWindow& window, Map& map, WaveManager& waveManager)
    : m_window(window), m_map(map), m_waveManager(waveManager),
      m_countdownTimer(0.f), m_timerView(m_countdownTimer)
{
    buildUI();

    if (!m_timerView.load("../assets/sprites/icons/timer.png",
                          "C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[GameView] TimerView: assets manquants.\n";

    // Icône petite (18px logiques)
    float iconSize = toWinH(18.f);
    m_timerView.setScale({ iconSize / 64.f, iconSize / 64.f });

    // Centré horizontalement en haut de la map, 8px de marge
    float tX = static_cast<float>(WIN_W) / 2.f - toWinW(20.f);
    float tY = 8.f;
    m_timerView.setPosition({ tX, tY });
}

// ─── makePanelShape ───────────────────────────────────────────────────────────
sf::RectangleShape GameView::makePanelShape(const sf::Texture& tex,
                                             float uiX, float uiY,
                                             float uiW, float uiH) const {
    sf::RectangleShape shape({ toWinW(uiW), toWinH(uiH) });
    shape.setPosition({ toWinX(uiX), toWinY(uiY) });
    shape.setTexture(&tex);
    shape.setFillColor(sf::Color::White);
    return shape;
}

// ─── buildUI ──────────────────────────────────────────────────────────────────
void GameView::buildUI() {
    loadTex(m_topPanelTex,   "../assets/sprites/buttons/top_panel.png");
    loadTex(m_goldPanelTex,  "../assets/sprites/buttons/gold_pannel.png");
    loadTex(m_heartPanelTex, "../assets/sprites/buttons/heart_pannel.png");

    m_topPanel   = makePanelShape(m_topPanelTex,    0.f,  0.f, 192.f, 32.f);
    m_goldPanel  = makePanelShape(m_goldPanelTex,  192.f, 16.f,  64.f, 32.f);
    m_heartPanel = makePanelShape(m_heartPanelTex, 192.f, 48.f,  64.f, 32.f);

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

    m_sellButton.emplace("../assets/sprites/buttons/sell_tower_button.png",
                         toWinX(128.f) + toWinW(64.f) / 2.f,
                         toWinY(32.f)  + toWinH(64.f) / 2.f,
                         toWinW(64.f), toWinH(64.f), MenuAction::None);

    m_backButton.emplace("../assets/sprites/buttons/back_to_main_menu_button.png",
                         toWinX(272.f) + toWinW(128.f) / 2.f,
                         toWinY(0.f)   + toWinH(32.f)  / 2.f,
                         toWinW(128.f), toWinH(32.f), MenuAction::Exit);
}

sf::View GameView::makeLetterboxView(unsigned screenW, unsigned screenH) {
    float contentRatio = static_cast<float>(WIN_W) / static_cast<float>(WIN_H);
    float screenRatio  = static_cast<float>(screenW) / static_cast<float>(screenH);

    float viewportW, viewportH, viewportX, viewportY;

    if (screenRatio > contentRatio) {
        viewportH = 1.f;
        viewportW = contentRatio / screenRatio;
        viewportX = (1.f - viewportW) / 2.f;
        viewportY = 0.f;
    } else {
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

void GameView::update(float dt) {
    m_waveManager.update(dt);
    m_timerView.update();
}

void GameView::render() {
    drawMap();
    drawEnemies();
    drawUIBar();
}

// ─── drawMap 
void GameView::drawMap() {
    m_map.draw(m_window, { 0.f, 0.f }, MAP_SCALE);
}

void GameView::drawEnemies() {
    for (const auto& enemy : m_waveManager.getActiveEnemies())
        enemy->render(m_window);
}

void GameView::drawUIBar() {
    m_window.draw(m_topPanel);
    m_window.draw(m_goldPanel);
    m_window.draw(m_heartPanel);

    for (const auto& btn : m_towerButtons)
        btn.draw(m_window);

    if (m_sellButton) m_sellButton->draw(m_window);
    if (m_backButton) m_backButton->draw(m_window);

    // Timer centré en haut de la map
    m_window.draw(m_timerView);
}

// ─── updateHover 
void GameView::updateHover(sf::Vector2f mousePos) {
    for (auto& btn : m_towerButtons)
        btn.setHovered(btn.contains(mousePos));

    if (m_sellButton) m_sellButton->setHovered(m_sellButton->contains(mousePos));
    if (m_backButton) m_backButton->setHovered(m_backButton->contains(mousePos));
}

void GameView::updateHoverAt(sf::Vector2f logicalPos) { updateHover(logicalPos); }

// ─── handleClickAt 
MenuAction GameView::handleClickAt(sf::Vector2f logicalPos) {
    for (const auto& btn : m_towerButtons)
        if (btn.contains(logicalPos)) return btn.getAction();

    if (m_sellButton && m_sellButton->contains(logicalPos))
        return m_sellButton->getAction();
    if (m_backButton && m_backButton->contains(logicalPos))
        return m_backButton->getAction();

    return MenuAction::None;
}

MenuAction GameView::handleEvent(const sf::Event& event) {
    return MenuAction::None;
}