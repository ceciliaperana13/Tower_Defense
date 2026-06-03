#include "GameView.hpp"
#include <iostream>

static void loadTex(sf::Texture& tex, const std::string& path) {
    if (!tex.loadFromFile(path))
        std::cerr << "[GameView] Missing texture: " << path << "\n";
}

GameView::GameView(sf::RenderWindow& window,
                   Map& map,
                   WaveManager& waveManager,
                   CountdownTimer& timer,
                   TowerController& towerController)
    : m_window(window)
    , m_map(map)
    , m_waveManager(waveManager)
    , m_timer(timer)
    , m_towerController(towerController)
    , m_timerView(timer)
{
    buildUI();

    // TimerView
    if (!m_timerView.load("../assets/sprites/icons/timer.png",
                          "C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[GameView] TimerView: missing assets.\n";

    m_timerView.setScale(sf::Vector2f(1.5f, 1.5f));
    m_timerView.setPosition(sf::Vector2f(WIN_W / 2.f - 32.f, 8.f));
}

sf::RectangleShape GameView::makePanelShape(const sf::Texture& tex,
                                            float uiX, float uiY,
                                            float uiW, float uiH) const {
    sf::RectangleShape shape({ toWinW(uiW), toWinH(uiH) });
    shape.setPosition({ toWinX(uiX), toWinY(uiY) });
    shape.setTexture(&tex);
    return shape;
}

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
    float contentRatio = float(WIN_W) / float(WIN_H);
    float screenRatio  = float(screenW) / float(screenH);

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

    // 🔥 AGRANDIR LA MAP
    sf::View view(sf::FloatRect(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(WIN_W, WIN_H * 1.35f)
    ));

    view.setViewport(sf::FloatRect(
        sf::Vector2f(viewportX, viewportY),
        sf::Vector2f(viewportW, viewportH)
    ));

    return view;
}

void GameView::update(float dt) {
    m_waveManager.update(dt);
    m_timer.update(dt);
    m_timerView.update();
}

void GameView::render() {
    drawMap();
    drawEnemies();
    drawUIBar();
}

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

    m_window.draw(m_timerView);
}

void GameView::updateHover(sf::Vector2f mousePos) {
    for (auto& btn : m_towerButtons)
        btn.setHovered(btn.contains(mousePos));

    if (m_sellButton) m_sellButton->setHovered(m_sellButton->contains(mousePos));
    if (m_backButton) m_backButton->setHovered(m_backButton->contains(mousePos));
}

void GameView::updateHoverAt(sf::Vector2f logicalPos) {
    updateHover(logicalPos);

    // 🔥 Déplacement du ghost
    if (!m_towerButtons.empty())
        m_towerController.setGhostPosition(logicalPos);
}

MenuAction GameView::handleClickAt(sf::Vector2f logicalPos) {
    // Boutons de tours → sélection
    for (size_t i = 0; i < m_towerButtons.size(); ++i) {
        if (m_towerButtons[i].contains(logicalPos)) {
            m_towerController.selectTower("basic");
            return MenuAction::None;
        }
    }

    // Placement tour
    if (m_towerController.hasSelection())
        m_towerController.placeTower(logicalPos);

    // Sell
    if (m_sellButton && m_sellButton->contains(logicalPos))
        return m_sellButton->getAction();

    // Back
    if (m_backButton && m_backButton->contains(logicalPos))
        return m_backButton->getAction();

    return MenuAction::None;
}

MenuAction GameView::handleEvent(const sf::Event& event) {
    return MenuAction::None;
}
