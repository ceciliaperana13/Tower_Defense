#include "GameView.hpp"
#include <iostream>

// Conversion logique → écran (identité)
static float toWinX(float x) { return x; }
static float toWinY(float y) { return y; }
static float toWinW(float w) { return w; }
static float toWinH(float h) { return h; }

static void loadTex(sf::Texture& tex, const std::string& path) {
    if (!tex.loadFromFile(path))
        std::cerr << "[GameView] Missing texture: " << path << "\n";
}

// ─────────────────────────────────────────────
// Constructeur
// ─────────────────────────────────────────────
GameView::GameView(sf::RenderWindow& window, Map& map,
                   WaveManager& waveManager, CountdownTimer& timer,
                   TowerController& towerController)
    : m_window(window)
    , m_map(map)
    , m_waveManager(waveManager)
    , m_countdownTimer(timer)
    , m_towerController(towerController)
    , m_timerView(timer)
    , m_mousePos(0.f, 0.f)
{
    buildUI();

    // Timer en haut
    if (!m_timerView.load("../assets/sprites/icons/timer.png",
                          "C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[GameView] TimerView: assets manquants.\n";

    float iconSize = toWinH(18.f);
    m_timerView.setScale({ iconSize / 64.f, iconSize / 64.f });

    m_timerView.setPosition(sf::Vector2f(
        WIN_W / 2.f - toWinW(20.f),
        8.f
    ));
}

// ─────────────────────────────────────────────
// makePanelShape
// ─────────────────────────────────────────────
sf::RectangleShape GameView::makePanelShape(const sf::Texture& tex,
                                            float uiX, float uiY,
                                            float uiW, float uiH) const {
    sf::RectangleShape shape(
        sf::Vector2f(toWinW(uiW), toWinH(uiH))
    );
    shape.setPosition(sf::Vector2f(toWinX(uiX), toWinY(uiY)));
    shape.setTexture(&tex);
    return shape;
}

// ─────────────────────────────────────────────
// buildUI — UI EN BAS
// ─────────────────────────────────────────────
void GameView::buildUI() {
    loadTex(m_topPanelTex,   "../assets/sprites/buttons/top_panel.png");
    loadTex(m_goldPanelTex,  "../assets/sprites/buttons/gold_pannel.png");
    loadTex(m_heartPanelTex, "../assets/sprites/buttons/heart_pannel.png");

    float baseY = MAP_H;

    m_topPanel   = makePanelShape(m_topPanelTex,    0.f,  baseY,     192.f, 32.f);
    m_goldPanel  = makePanelShape(m_goldPanelTex,  192.f, baseY+16.f, 64.f, 32.f);
    m_heartPanel = makePanelShape(m_heartPanelTex, 192.f, baseY+48.f, 64.f, 32.f);

    // Zone UI
    m_uiRect = sf::FloatRect(
        sf::Vector2f(0.f, MAP_H),
        sf::Vector2f(WIN_W, WIN_H - MAP_H)
    );

    struct BtnDef { const char* path; float x, y, w, h; };

    BtnDef defs[] = {
        { "../assets/sprites/buttons/buy_normal_tower_button.png",  0.f,  baseY+32.f, 64.f, 64.f },
        { "../assets/sprites/buttons/fire_button.png",             64.f, baseY+32.f, 32.f, 32.f },
        { "../assets/sprites/buttons/ice_button.png",              96.f, baseY+32.f, 32.f, 32.f },
        { "../assets/sprites/buttons/earth_button.png",            64.f, baseY+64.f, 32.f, 32.f },
        { "../assets/sprites/buttons/arcane_button.png",           96.f, baseY+64.f, 32.f, 32.f },
    };

    m_towerButtons.reserve(5);
    for (int i = 0; i < 5; ++i) {
        auto& d = defs[i];
        float cx = d.x + d.w / 2.f;
        float cy = d.y + d.h / 2.f;

        m_towerButtons.emplace_back(
            d.path, cx, cy, d.w, d.h, MenuAction::None
        );
    }

    m_sellButton.emplace("../assets/sprites/buttons/sell_tower_button.png",
                         128.f + 32.f, baseY + 32.f + 32.f,
                         64.f, 64.f, MenuAction::None);

    m_backButton.emplace("../assets/sprites/buttons/back_to_main_menu_button.png",
                         272.f + 64.f, baseY + 16.f,
                         128.f, 32.f, MenuAction::Exit);
}

// ─────────────────────────────────────────────
// Letterboxing SFML3
// ─────────────────────────────────────────────
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

    sf::View view(sf::FloatRect(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(WIN_W, WIN_H)
    ));

    view.setViewport(sf::FloatRect(
        sf::Vector2f(viewportX, viewportY),
        sf::Vector2f(viewportW, viewportH)
    ));

    return view;
}

// ─────────────────────────────────────────────
// update
// ─────────────────────────────────────────────
void GameView::update(float dt) {
    m_waveManager.update(dt);
    m_timerView.update();
}

// ─────────────────────────────────────────────
// render
// ─────────────────────────────────────────────
void GameView::render() {
    drawMap();
    drawEnemies();
    drawTowers();
    drawUIBar();
}

// ─────────────────────────────────────────────
// drawMap
// ─────────────────────────────────────────────
void GameView::drawMap() {
    m_map.draw(m_window, { 0.f, 0.f }, MAP_SCALE);
}

// ─────────────────────────────────────────────
// drawEnemies
// ─────────────────────────────────────────────
void GameView::drawEnemies() {
    for (const auto& enemy : m_waveManager.getActiveEnemies())
        enemy->render(m_window);
}

// ─────────────────────────────────────────────
// drawTowers
// ─────────────────────────────────────────────
void GameView::drawTowers() {
    m_towerController.drawPlaced(m_window);

    if (!isOverUI(m_mousePos))
        m_towerController.drawGhost(m_window, m_mousePos);
}

// ─────────────────────────────────────────────
// drawUIBar
// ─────────────────────────────────────────────
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

// ─────────────────────────────────────────────
// isOverUI
// ─────────────────────────────────────────────
bool GameView::isOverUI(sf::Vector2f logicalPos) const {
    return m_uiRect.contains(logicalPos);
}

// ─────────────────────────────────────────────
// updateHover
// ─────────────────────────────────────────────
void GameView::updateHover(sf::Vector2f mousePos) {
    for (auto& btn : m_towerButtons)
        btn.setHovered(btn.contains(mousePos));

    if (m_sellButton) m_sellButton->setHovered(m_sellButton->contains(mousePos));
    if (m_backButton) m_backButton->setHovered(m_backButton->contains(mousePos));
}

void GameView::updateHoverAt(sf::Vector2f logicalPos) {
    m_mousePos = logicalPos;
    updateHover(logicalPos);
}

// ─────────────────────────────────────────────
// handleClickAt
// ─────────────────────────────────────────────
MenuAction GameView::handleClickAt(sf::Vector2f logicalPos) {
    for (int i = 0; i < 5; ++i) {
        if (m_towerButtons[i].contains(logicalPos)) {
            m_towerController.selectTower(TOWER_NAMES[i]);
            return MenuAction::None;
        }
    }

    if (m_sellButton && m_sellButton->contains(logicalPos))
        return m_sellButton->getAction();

    if (m_backButton && m_backButton->contains(logicalPos))
        return m_backButton->getAction();

    if (!isOverUI(logicalPos) && m_towerController.hasSelection()) {
        m_towerController.placeTower(logicalPos);
        return MenuAction::None;
    }

    return MenuAction::None;
}

// ─────────────────────────────────────────────
// handleEvent
// ─────────────────────────────────────────────
MenuAction GameView::handleEvent(const sf::Event& event) {
    if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = m_window.mapPixelToCoords(mb->position);
            return handleClickAt(pos);
        }
    }

    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape)
            m_towerController.deselect();
    }

    return MenuAction::None;
}
