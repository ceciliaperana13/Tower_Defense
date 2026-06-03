#include "GameView.hpp"
#include <iostream>

static void loadTex(sf::Texture& tex, const std::string& path) {
    if (!tex.loadFromFile(path))
        std::cerr << "[GameView] Missing texture: " << path << "\n";
}

// ─── Constructeur ─────────────────────────────────────────────────────────────
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

    if (!m_timerView.load("../assets/sprites/icons/timer.png",
                          "C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[GameView] TimerView: missing assets.\n";

    m_timerView.setScale(sf::Vector2f(1.5f, 1.5f));
    // Centré horizontalement, dans la zone carte
    m_timerView.setPosition(sf::Vector2f(float(WIN_W) / 2.f - 48.f, 8.f));
}

// ─── makePanelShape ───────────────────────────────────────────────────────────
sf::RectangleShape GameView::makePanelShape(const sf::Texture& tex,
                                            float px, float py,
                                            float pw, float ph) const {
    sf::RectangleShape shape({ pw, ph });
    shape.setPosition({ px, py });
    shape.setTexture(&tex);
    shape.setFillColor(sf::Color::White);
    return shape;
}

// ─── buildUI ──────────────────────────────────────────────────────────────────
void GameView::buildUI() {
    loadTex(m_topPanelTex,   "../assets/sprites/buttons/top_panel.png");
    loadTex(m_goldPanelTex,  "../assets/sprites/buttons/gold_pannel.png");
    loadTex(m_heartPanelTex, "../assets/sprites/buttons/heart_pannel.png");

    // ── Panels ───────────────────────────────────────────────────────────────
    // Panel principal BUILD : 0→192 logique = toute la zone gauche
    m_topPanel = makePanelShape(
        m_topPanelTex,
        0.f,           MAP_H,
        toWinW(192.f), UI_H
    );
    // Gold : demi-hauteur supérieure, après le panel principal
    m_goldPanel = makePanelShape(
        m_goldPanelTex,
        toWinX(192.f), MAP_H,
        toWinW(64.f),  UI_H / 2.f
    );
    // Heart : demi-hauteur inférieure
    m_heartPanel = makePanelShape(
        m_heartPanelTex,
        toWinX(192.f), MAP_H + UI_H / 2.f,
        toWinW(64.f),  UI_H / 2.f
    );

    // ── Boutons tours ────────────────────────────────────────────────────────
    // Espace logique 320x96 — on remplit toute la hauteur UI
    struct BtnDef { const char* path; float x, y, w, h; const char* type; };
    constexpr BtnDef defs[] = {
        // Bouton BUILD (tour basique) : occupe colonne 0-64 pleine hauteur
        { "../assets/sprites/buttons/buy_normal_tower_button.png",  0.f,  0.f, 64.f, 96.f, "basic"  },
        // 4 boutons upgrade : colonne 64-128, rangés 2x2
        { "../assets/sprites/buttons/fire_button.png",             64.f,  0.f, 32.f, 48.f, "fire"   },
        { "../assets/sprites/buttons/ice_button.png",              96.f,  0.f, 32.f, 48.f, "ice"    },
        { "../assets/sprites/buttons/earth_button.png",            64.f, 48.f, 32.f, 48.f, "rock"   },
        { "../assets/sprites/buttons/arcane_button.png",           96.f, 48.f, 32.f, 48.f, "arcane" },
    };

    m_towerButtons.reserve(5);
    m_towerTypes.reserve(5);
    for (const auto& d : defs) {
        float cx = toWinX(d.x) + toWinW(d.w) / 2.f;
        float cy = toUIY(d.y)  + toUIH(d.h)  / 2.f;
        m_towerButtons.emplace_back(d.path, cx, cy,
                                    toWinW(d.w), toUIH(d.h),
                                    MenuAction::None);
        m_towerTypes.push_back(d.type);
    }

    // ── SELL : colonne 128-192, pleine hauteur ────────────────────────────────
    m_sellButton.emplace(
        "../assets/sprites/buttons/sell_tower_button.png",
        toWinX(128.f) + toWinW(64.f) / 2.f,
        toUIY(0.f)    + toUIH(96.f)  / 2.f,
        toWinW(64.f),  toUIH(96.f),
        MenuAction::None
    );

    // ── BACK : ancré à droite, colonne 272-320 x pleine hauteur ──────────────
    // On utilise WIN_W pour le coller au bord droit
    float backW = toWinW(128.f);
    m_backButton.emplace(
        "../assets/sprites/buttons/back_to_main_menu_button.png",
        float(WIN_W) - backW / 2.f - (float(WIN_W) - toWinX(320.f)) / 2.f,
        toUIY(0.f) + toUIH(96.f) / 2.f,
        backW, toUIH(96.f),
        MenuAction::Exit
    );
}

// ─── makeLetterboxView ────────────────────────────────────────────────────────
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
        sf::Vector2f(float(WIN_W), float(WIN_H))
    ));
    view.setViewport(sf::FloatRect(
        sf::Vector2f(viewportX, viewportY),
        sf::Vector2f(viewportW, viewportH)
    ));
    return view;
}

// ─── update ───────────────────────────────────────────────────────────────────
void GameView::update(float /*dt*/) {
    m_timerView.update();
}

// ─── render ───────────────────────────────────────────────────────────────────
void GameView::render() {
    drawMap();
    drawEnemies();
    drawUIBar();
    m_window.draw(m_timerView);
}

// ─── drawMap ──────────────────────────────────────────────────────────────────
void GameView::drawMap() {
    m_map.draw(m_window, { 0.f, 0.f }, MAP_SCALE);
}

// ─── drawEnemies ──────────────────────────────────────────────────────────────
void GameView::drawEnemies() {
    for (const auto& enemy : m_waveManager.getActiveEnemies())
        enemy->render(m_window);
}

// ─── drawUIBar ────────────────────────────────────────────────────────────────
void GameView::drawUIBar() {
    m_window.draw(m_topPanel);
    m_window.draw(m_goldPanel);
    m_window.draw(m_heartPanel);

    for (const auto& btn : m_towerButtons)
        btn.draw(m_window);

    if (m_sellButton) m_sellButton->draw(m_window);
    if (m_backButton) m_backButton->draw(m_window);
}

// ─── getTowerTypeAt ───────────────────────────────────────────────────────────
std::string GameView::getTowerTypeAt(sf::Vector2f logicalPos) const {
    for (std::size_t i = 0; i < m_towerButtons.size(); ++i)
        if (m_towerButtons[i].contains(logicalPos))
            return m_towerTypes[i];
    return "";
}

// ─── updateHover ──────────────────────────────────────────────────────────────
void GameView::updateHover(sf::Vector2f mousePos) {
    for (auto& btn : m_towerButtons)
        btn.setHovered(btn.contains(mousePos));
    if (m_sellButton) m_sellButton->setHovered(m_sellButton->contains(mousePos));
    if (m_backButton) m_backButton->setHovered(m_backButton->contains(mousePos));
}

void GameView::updateHoverAt(sf::Vector2f logicalPos) { updateHover(logicalPos); }

// ─── handleClickAt ────────────────────────────────────────────────────────────
MenuAction GameView::handleClickAt(sf::Vector2f logicalPos) {
    // Les boutons tours sont traités via getTowerTypeAt dans main.cpp
    for (const auto& btn : m_towerButtons)
        if (btn.contains(logicalPos)) return MenuAction::None;

    if (m_sellButton && m_sellButton->contains(logicalPos))
        return m_sellButton->getAction();
    if (m_backButton && m_backButton->contains(logicalPos))
        return m_backButton->getAction();

    return MenuAction::None;
}

// ─── handleEvent ──────────────────────────────────────────────────────────────
MenuAction GameView::handleEvent(const sf::Event& /*event*/) {
    return MenuAction::None;
}