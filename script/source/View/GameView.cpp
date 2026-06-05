#include "View/GameView.hpp"
#include <iostream>
#include <string>

static void loadTex(sf::Texture& tex, const std::string& path) {
    if (!tex.loadFromFile(path))
        std::cerr << "[GameView] Missing texture: " << path << "\n";
}
//GameView
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
    , m_waveView(waveManager)
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
    m_waveView.setPosition(sf::Vector2f(float(WIN_W) / 2.f + 180.f, 14.f));

    m_upgradeHighlight.setRadius(28.f);
    m_upgradeHighlight.setOrigin({ 28.f, 28.f });
    m_upgradeHighlight.setFillColor(sf::Color::Transparent);
    m_upgradeHighlight.setOutlineColor(sf::Color::Yellow);
    m_upgradeHighlight.setOutlineThickness(3.f);
}

sf::RectangleShape GameView::makePanelShape(const sf::Texture& tex,
                                            float px, float py,
                                            float pw, float ph) const {
    sf::RectangleShape shape({ pw, ph });
    shape.setPosition({ px, py });
    shape.setTexture(&tex);
    shape.setFillColor(sf::Color::White);
    return shape;
}
//
void GameView::buildUI() {
    loadTex(m_topPanelTex,   "../assets/sprites/buttons/top_panel.png");
    loadTex(m_goldPanelTex,  "../assets/sprites/buttons/gold_pannel.png");
    loadTex(m_heartPanelTex, "../assets/sprites/buttons/heart_pannel.png");

    m_topPanel = makePanelShape(m_topPanelTex, 0.f, MAP_H, toWinW(192.f), UI_H);

    float gx = toWinX(192.f);
    float gw = toWinW(64.f);
    float gh = toUIH(48.f);

    m_goldPanel  = makePanelShape(m_goldPanelTex,  gx, MAP_H,      gw, gh);
    m_heartPanel = makePanelShape(m_heartPanelTex, gx, MAP_H + gh, gw, gh);

    unsigned int charSize = 22u;
    float textX = gx + gw * 0.18f;
    float padY  = (gh - float(charSize)) / 2.f;

    m_coinsText.setFont(m_font);
    m_coinsText.setCharacterSize(charSize);
    m_coinsText.setFillColor(sf::Color(255, 220, 0));
    m_coinsText.setStyle(sf::Text::Bold);
    m_coinsText.setString("1");
    m_coinsText.setPosition({ textX, MAP_H + padY });

    m_livesText.setFont(m_font);
    m_livesText.setCharacterSize(charSize);
    m_livesText.setFillColor(sf::Color(255, 80, 80));
    m_livesText.setStyle(sf::Text::Bold);
    m_livesText.setString("20");
    m_livesText.setPosition({ textX, MAP_H + gh + padY });

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

    m_sellButton.emplace(
        "../assets/sprites/buttons/sell_tower_button.png",
        toWinX(128.f) + toWinW(64.f) / 2.f,
        toUIY(0.f)    + toUIH(96.f)  / 2.f,
        toWinW(64.f), toUIH(96.f),
        MenuAction::SellTower
    );

    float backW = toWinW(64.f);
    float backX = toWinX(256.f) + backW / 2.f;
    m_backButton.emplace(
        "../assets/sprites/buttons/back_to_main_menu_button.png",
        backX, toUIY(0.f) + toUIH(96.f) / 2.f,
        backW, toUIH(96.f),
        MenuAction::Exit
    );
}

// ─── Popup confirmation 
void GameView::showConfirm(const std::string& towerType,
                           bool isUpgrade,
                           sf::Vector2f mapPos) {
    m_confirmType      = towerType;
    m_confirmIsUpgrade = isUpgrade;
    m_confirmMapPos    = mapPos;
    m_confirmPending   = true;
    buildConfirmPopup();
}

void GameView::cancelConfirm() {
    m_confirmPending = false;
}

void GameView::buildConfirmPopup() {
    // Popup juste au-dessus de la barre UI
    constexpr float PW = 320.f;
    constexpr float PH = 90.f;
    float px = (float(WIN_W) - PW) / 2.f;
    float py = MAP_H - PH - 8.f;   // juste au-dessus de l'UI

    m_confirmPanel.setSize({ PW, PH });
    m_confirmPanel.setPosition({ px, py });
    m_confirmPanel.setFillColor(sf::Color(15, 15, 25, 235));
    m_confirmPanel.setOutlineColor(sf::Color(180, 140, 60));
    m_confirmPanel.setOutlineThickness(2.f);

    int cost = m_towerController.getCostOf(m_confirmType);
    bool canAfford = m_towerController.getCoins() >= cost;

    std::string action = m_confirmIsUpgrade ? "Upgrader" : "Placer";
    std::string title  = action + " " + m_confirmType +
                         " (" + std::to_string(cost) + " coins) ?";

    m_confirmTitle.emplace(m_font, title, 16u);
    m_confirmTitle->setFillColor(canAfford
        ? sf::Color(220, 220, 220)
        : sf::Color(220, 80, 80));
    sf::FloatRect tb = m_confirmTitle->getLocalBounds();
    m_confirmTitle->setPosition({
        px + (PW - tb.size.x) / 2.f - tb.position.x,
        py + 10.f
    });

    // Bouton Oui
    m_confirmYesBtn.setSize({ 110.f, 36.f });
    m_confirmYesBtn.setPosition({ px + 20.f, py + PH - 46.f });
    m_confirmYesBtn.setFillColor(canAfford
        ? sf::Color(40, 120, 40)
        : sf::Color(60, 60, 60));
    m_confirmYesBtn.setOutlineColor(sf::Color(180, 140, 60));
    m_confirmYesBtn.setOutlineThickness(1.5f);

    m_confirmYesLabel.emplace(m_font, canAfford ? "Oui" : "Pas assez", 16u);
    m_confirmYesLabel->setFillColor(sf::Color::White);
    centerText(*m_confirmYesLabel, m_confirmYesBtn);

    // Bouton Non
    m_confirmNoBtn.setSize({ 110.f, 36.f });
    m_confirmNoBtn.setPosition({ px + PW - 130.f, py + PH - 46.f });
    m_confirmNoBtn.setFillColor(sf::Color(120, 40, 40));
    m_confirmNoBtn.setOutlineColor(sf::Color(180, 140, 60));
    m_confirmNoBtn.setOutlineThickness(1.5f);

    m_confirmNoLabel.emplace(m_font, "Non", 16u);
    m_confirmNoLabel->setFillColor(sf::Color::White);
    centerText(*m_confirmNoLabel, m_confirmNoBtn);
}

void GameView::handleConfirmClick(sf::Vector2f pos) {
    if (!m_confirmPending) return;

    if (m_confirmYesBtn.getGlobalBounds().contains(pos)) {
        int cost = m_towerController.getCostOf(m_confirmType);
        if (m_towerController.getCoins() >= cost) {
            if (m_confirmIsUpgrade) {
                m_towerController.upgradeTower(m_confirmType);
            } else {
                // Sélectionne et place directement
                m_towerController.selectTower(m_confirmType);
                m_towerController.placeTower(m_confirmMapPos);
            }
        }
        m_confirmPending = false;
        return;
    }

    if (m_confirmNoBtn.getGlobalBounds().contains(pos)) {
        m_towerController.clearSelection();
        m_confirmPending = false;
    }
}

void GameView::drawConfirmPopup() {
    if (!m_confirmPending) return;

    m_window.draw(m_confirmPanel);
    if (m_confirmTitle)    m_window.draw(*m_confirmTitle);
    m_window.draw(m_confirmYesBtn);
    if (m_confirmYesLabel) m_window.draw(*m_confirmYesLabel);
    m_window.draw(m_confirmNoBtn);
    if (m_confirmNoLabel)  m_window.draw(*m_confirmNoLabel);
}

void GameView::centerText(sf::Text& text,
                          const sf::RectangleShape& btn) const {
    sf::FloatRect tb  = text.getLocalBounds();
    sf::Vector2f  pos = btn.getPosition();
    sf::Vector2f  sz  = btn.getSize();
    text.setPosition({
        pos.x + (sz.x - tb.size.x) / 2.f - tb.position.x,
        pos.y + (sz.y - tb.size.y) / 2.f - tb.position.y
    });
}

// ─── makeLetterboxView 
sf::View GameView::makeLetterboxView(unsigned screenW, unsigned screenH) {
    float contentRatio = float(WIN_W) / float(WIN_H);
    float screenRatio  = float(screenW) / float(screenH);
    float viewportW, viewportH, viewportX, viewportY;
    if (screenRatio > contentRatio) {
        viewportH = 1.f; viewportW = contentRatio / screenRatio;
        viewportX = (1.f - viewportW) / 2.f; viewportY = 0.f;
    } else {
        viewportW = 1.f; viewportH = screenRatio / contentRatio;
        viewportX = 0.f; viewportY = (1.f - viewportH) / 2.f;
    }
    sf::View view(sf::FloatRect(sf::Vector2f(0.f, 0.f),
                                sf::Vector2f(float(WIN_W), float(WIN_H))));
    view.setViewport(sf::FloatRect(sf::Vector2f(viewportX, viewportY),
                                   sf::Vector2f(viewportW, viewportH)));
    return view;
}

// ─── updateTexts 
void GameView::updateTexts() {
    m_coinsText.setString(std::to_string(m_towerController.getCoins()));
    m_livesText.setString(std::to_string(m_lives));
}

// ─── update 
void GameView::update(float /*dt*/) {
    m_timerView.update();
    m_waveView.update();
    updateTexts();
}

// ─── render 
void GameView::render() {
    drawMap();
    drawEnemies();
    drawUpgradeHighlight();
    drawUIBar();
    drawConfirmPopup();           //  popup par-dessus l'UI
    m_window.draw(m_timerView);
    m_waveView.render(m_window);
}

void GameView::drawMap() {
    m_map.draw(m_window, { 0.f, 0.f }, MAP_SCALE);
}

void GameView::drawEnemies() {
    for (const auto& enemy : m_waveManager.getActiveEnemies())
        enemy->render(m_window);
}

void GameView::drawUpgradeHighlight() {
    if (!m_towerController.hasUpgradeTarget()) return;
}
//UI
void GameView::drawUIBar() {
    m_window.draw(m_topPanel);
    m_window.draw(m_goldPanel);
    m_window.draw(m_heartPanel);
    m_window.draw(m_coinsText);
    m_window.draw(m_livesText);
    for (const auto& btn : m_towerButtons)
        btn.draw(m_window);
    if (m_sellButton) m_sellButton->draw(m_window);
    if (m_backButton) m_backButton->draw(m_window);
}

std::string GameView::getTowerTypeAt(sf::Vector2f logicalPos) const {
    for (std::size_t i = 0; i < m_towerButtons.size(); ++i)
        if (m_towerButtons[i].contains(logicalPos))
            return m_towerTypes[i];
    return "";
}
// ─── updateHover 
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
        if (btn.contains(logicalPos)) return MenuAction::None;
    if (m_sellButton && m_sellButton->contains(logicalPos))
        return m_sellButton->getAction();
    if (m_backButton && m_backButton->contains(logicalPos))
        return m_backButton->getAction();
    return MenuAction::None;
}

MenuAction GameView::handleEvent(const sf::Event& /*event*/) {
    return MenuAction::None;
}

void GameView::drawPriceTags() {}