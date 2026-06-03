#include "GameView.hpp"
#include <iostream>
#include <string>

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
    , m_coinsText(m_font)
    , m_livesText(m_font)
{
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[GameView] Font not found.\n";

    buildUI();

    if (!m_timerView.load("../assets/sprites/icons/timer.png",
                          "C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[GameView] TimerView: missing assets.\n";

    m_timerView.setScale(sf::Vector2f(1.5f, 1.5f));
    m_timerView.setPosition(sf::Vector2f(float(WIN_W) / 2.f - 48.f, 8.f));

    m_upgradeHighlight.setRadius(28.f);
    m_upgradeHighlight.setOrigin({ 28.f, 28.f });
    m_upgradeHighlight.setFillColor(sf::Color::Transparent);
    m_upgradeHighlight.setOutlineColor(sf::Color::Yellow);
    m_upgradeHighlight.setOutlineThickness(3.f);
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

    // ── Panels fond ──────────────────────────────────────────────────────────
    m_topPanel = makePanelShape(
        m_topPanelTex, 0.f, MAP_H, toWinW(192.f), UI_H);

    float panelX = toWinX(192.f);
    float panelW = toWinW(64.f);
    float halfH  = UI_H / 2.f;

    m_goldPanel  = makePanelShape(m_goldPanelTex,
        panelX, MAP_H,          panelW, halfH);
    m_heartPanel = makePanelShape(m_heartPanelTex,
        panelX, MAP_H + halfH,  panelW, halfH);

    // ── Textes coins / vies ───────────────────────────────────────────────────
    // Les images gold_pannel et heart_pannel ont une icône à gauche (environ 40px)
    // On place le texte à droite de l'icône, centré verticalement dans chaque demi-panel
    unsigned int charSize = static_cast<unsigned int>(halfH * 0.45f); // ~28px pour UI_H=128
    float iconOffset = panelW * 0.42f;   // ~40% de la largeur = zone icône
    float textX      = panelX + iconOffset;

    m_coinsText.setCharacterSize(charSize);
    m_coinsText.setFillColor(sf::Color(255, 215, 0));   // or
    m_coinsText.setStyle(sf::Text::Bold);
    m_coinsText.setPosition({
        textX,
        MAP_H + (halfH - float(charSize)) / 2.f
    });

    m_livesText.setCharacterSize(charSize);
    m_livesText.setFillColor(sf::Color(230, 60, 60));   // rouge vif
    m_livesText.setStyle(sf::Text::Bold);
    m_livesText.setPosition({
        textX,
        MAP_H + halfH + (halfH - float(charSize)) / 2.f
    });

    // ── Boutons tours ─────────────────────────────────────────────────────────
    struct BtnDef { const char* path; float x, y, w, h; const char* type; };
    constexpr BtnDef defs[] = {
        { "../assets/sprites/buttons/buy_normal_tower_button.png",  0.f,  0.f, 64.f, 96.f, "basic"  },
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

    // ── SELL ──────────────────────────────────────────────────────────────────
    m_sellButton.emplace(
        "../assets/sprites/buttons/sell_tower_button.png",
        toWinX(128.f) + toWinW(64.f) / 2.f,
        toUIY(0.f)    + toUIH(96.f)  / 2.f,
        toWinW(64.f),  toUIH(96.f),
        MenuAction::None
    );

    // ── BACK ──────────────────────────────────────────────────────────────────
    float backW = toWinW(128.f);
    float backX = float(WIN_W) - backW / 2.f;
    m_backButton.emplace(
        "../assets/sprites/buttons/back_to_main_menu_button.png",
        backX, toUIY(0.f) + toUIH(96.f) / 2.f,
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

// ─── updateTexts ──────────────────────────────────────────────────────────────
void GameView::updateTexts() {
    // Coins : récupéré depuis TowerController
    m_coinsText.setString(std::to_string(m_towerController.getCoins()));

    // Vies : setLives() appelé depuis main à chaque fois qu'un ennemi atteint le château
    m_livesText.setString(std::to_string(m_lives));
}

// ─── update ───────────────────────────────────────────────────────────────────
void GameView::update(float /*dt*/) {
    m_timerView.update();
    updateTexts();
}

// ─── render ───────────────────────────────────────────────────────────────────
void GameView::render() {
    drawMap();
    drawEnemies();
    drawUpgradeHighlight();
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

// ─── drawUpgradeHighlight ─────────────────────────────────────────────────────
void GameView::drawUpgradeHighlight() {
    if (!m_towerController.hasUpgradeTarget()) return;
    // Le ring est positionné depuis main.cpp via upgradeRingPos
}

// ─── drawUIBar ────────────────────────────────────────────────────────────────
void GameView::drawUIBar() {
    // 1. Fond des panels
    m_window.draw(m_topPanel);
    m_window.draw(m_goldPanel);
    m_window.draw(m_heartPanel);

    // 2. Textes par-dessus (coins en or, vies en rouge)
    m_window.draw(m_coinsText);
    m_window.draw(m_livesText);

    // 3. Boutons tours
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