#include "GameView.hpp"
#include <iostream>

// Helpers simples
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
                   WaveManager& waveManager, CountdownTimer& timer)
    : m_window(window),
      m_map(map),
      m_waveManager(waveManager),
      m_countdownTimer(timer),
      m_timerView(timer),
      m_towerController()
{
    buildUI();

    // TIMER EN HAUT — COMME AVANT
    if (!m_timerView.load("../assets/sprites/icons/timer.png",
                          "C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[GameView] TimerView: assets manquants.\n";

    float iconSize = 32.f;
    m_timerView.setScale({ iconSize / 64.f, iconSize / 64.f });

    m_timerView.setPosition({ WIN_W / 2.f - 20.f, 8.f });
}

// ─────────────────────────────────────────────
// makePanelShape
// ─────────────────────────────────────────────
sf::RectangleShape GameView::makePanelShape(const sf::Texture& tex,
                                            float uiX, float uiY,
                                            float uiW, float uiH) const {
    sf::RectangleShape shape({ uiW, uiH });
    shape.setPosition({ uiX, uiY });
    shape.setTexture(&tex);
    return shape;
}

// ─────────────────────────────────────────────
// buildUI — UI EN BAS COMME AVANT
// ─────────────────────────────────────────────
void GameView::buildUI() {
    loadTex(m_topPanelTex,   "../assets/sprites/buttons/top_panel.png");
    loadTex(m_goldPanelTex,  "../assets/sprites/buttons/gold_pannel.png");
    loadTex(m_heartPanelTex, "../assets/sprites/buttons/heart_pannel.png");

    float baseY = WIN_H - 96.f;

    m_topPanel   = makePanelShape(m_topPanelTex,    0.f,      baseY,     192.f, 32.f);
    m_goldPanel  = makePanelShape(m_goldPanelTex,  192.f, baseY + 16.f,  64.f, 32.f);
    m_heartPanel = makePanelShape(m_heartPanelTex, 192.f, baseY + 48.f,  64.f, 32.f);

    // Boutons tours — EXACTEMENT COMME AVANT MAIS EN BAS
    struct BtnDef { const char* path; float x, y, w, h; };
    BtnDef defs[]= {
        { "../assets/sprites/buttons/buy_normal_tower_button.png",  0.f,  baseY+32.f, 64.f, 64.f },
        { "../assets/sprites/buttons/fire_button.png",             64.f, baseY+32.f, 32.f, 32.f },
        { "../assets/sprites/buttons/ice_button.png",              96.f, baseY+32.f, 32.f, 32.f },
        { "../assets/sprites/buttons/earth_button.png",            64.f, baseY+64.f, 32.f, 32.f },
        { "../assets/sprites/buttons/arcane_button.png",           96.f, baseY+64.f, 32.f, 32.f },
    };

    m_towerButtons.reserve(5);
    int index = 0;
    for (const auto& d : defs) {
        MenuAction action = MenuAction::None;
        if (index == 0) action = MenuAction::BasicTower; // bouton basic

        m_towerButtons.emplace_back(
            d.path,
            d.x + d.w / 2.f,
            d.y + d.h / 2.f,
            d.w, d.h,
            action
        );
        index++;
    }

    // Bouton SELL
    m_sellButton.emplace("../assets/sprites/buttons/sell_tower_button.png",
                         128.f + 32.f, baseY + 32.f + 32.f,
                         64.f, 64.f, MenuAction::None);

    // Bouton BACK
    m_backButton.emplace("../assets/sprites/buttons/back_to_main_menu_button.png",
                         272.f + 64.f, baseY + 16.f,
                         128.f, 32.f, MenuAction::Exit);
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
    drawUIBar();

    // Tours placées
    m_towerController.drawPlaced(m_window);

    // Ghost
    sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
    sf::Vector2f worldPos = m_window.mapPixelToCoords(mousePos);
    m_towerController.drawGhost(m_window, worldPos);
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

    // TIMER EN HAUT
    m_window.draw(m_timerView);
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

void GameView::updateHoverAt(sf::Vector2f logicalPos) { updateHover(logicalPos); }

// ─────────────────────────────────────────────
// handleClickAt
// ─────────────────────────────────────────────
MenuAction GameView::handleClickAt(sf::Vector2f logicalPos) {
    for (const auto& btn : m_towerButtons)
        if (btn.contains(logicalPos)) return btn.getAction();

    if (m_sellButton && m_sellButton->contains(logicalPos))
        return m_sellButton->getAction();
    if (m_backButton && m_backButton->contains(logicalPos))
        return m_backButton->getAction();

    return MenuAction::None;
}

// ─────────────────────────────────────────────
// handleEvent — SFML 3 + placement tours
// ─────────────────────────────────────────────
MenuAction GameView::handleEvent(const sf::Event& event) {
    if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = m_window.mapPixelToCoords(mb->position);

            MenuAction action = handleClickAt(pos);

            switch (action) {
            case MenuAction::BasicTower:
                m_towerController.selectTower("basic");
                return action;

            case MenuAction::None:
                if (m_towerController.hasSelection())
                    m_towerController.placeTower(pos);
                return action;

            default:
                return action;
            }
        }
    }

    return MenuAction::None;
}
