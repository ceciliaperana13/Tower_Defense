#include "State/GameOverState.hpp"
#include "State/MenuState.hpp"
#include "State/GameState.hpp"
#include "Game.hpp"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <algorithm>

GameOverState::GameOverState(sf::RenderWindow& window,
                             int score, int kills, int wave)
    : m_window(window), m_score(score), m_kills(kills), m_wave(wave)
{
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[GameOverState] Font not found.\n";
    buildUI();
}

void GameOverState::onEnter() {
    m_fadeAlpha = 255.f;
    m_sliding   = true;
}

void GameOverState::buildUI() {
    const float W  = float(WIN_W);
    const float H  = float(WIN_H);
    const float pw = 460.f;
    const float ph = 340.f;
    const float px = (W - pw) / 2.f;

    m_panelTargetY = (H - ph) / 2.f;
    m_panelY       = H + ph;   // démarre hors écran bas (slide depuis le bas)

    m_panel.setSize({ pw, ph });
    m_panel.setPosition({ px, m_panelY });
    m_panel.setFillColor(sf::Color(25, 5, 5, 245));
    m_panel.setOutlineColor(sf::Color(200, 40, 40));
    m_panel.setOutlineThickness(3.f);

    // ── Titre ────────────────────────────────────────────────
    m_titleText.emplace(m_font, "DEFEAT", 54u);
    m_titleText->setFillColor(sf::Color(220, 40, 40));
    m_titleText->setStyle(sf::Text::Bold);

    // ── Stats ────────────────────────────────────────────────
    m_scoreLabel.emplace(m_font, "Score  :  0", 24u);
    m_scoreLabel->setFillColor(sf::Color(255, 160, 60));

    m_killsLabel.emplace(m_font, "Kills   :  0", 24u);
    m_killsLabel->setFillColor(sf::Color(210, 210, 210));

    m_waveLabel.emplace(m_font, "Wave :  0", 24u);
    m_waveLabel->setFillColor(sf::Color(210, 210, 210));

    // ── Bouton Rejouer ───────────────────────────────────────
    m_replayBtn.setSize({ 180.f, 48.f });
    m_replayBtn.setFillColor(sf::Color(80, 20, 20));
    m_replayBtn.setOutlineColor(sf::Color(200, 60, 60));
    m_replayBtn.setOutlineThickness(2.f);

    m_replayLabel.emplace(m_font, "Play again", 20u);
    m_replayLabel->setFillColor(sf::Color::White);

    // ── Bouton Menu ──────────────────────────────────────────
    m_menuBtn.setSize({ 180.f, 48.f });
    m_menuBtn.setFillColor(sf::Color(40, 40, 60));
    m_menuBtn.setOutlineColor(sf::Color(180, 140, 60));
    m_menuBtn.setOutlineThickness(2.f);

    m_menuLabel.emplace(m_font, "Menu", 20u);
    m_menuLabel->setFillColor(sf::Color::White);
}

static void centerText(sf::Text& text, sf::FloatRect rect) {
    sf::FloatRect tb = text.getLocalBounds();
    text.setPosition({
        rect.position.x + (rect.size.x - tb.size.x) / 2.f - tb.position.x,
        rect.position.y + (rect.size.y - tb.size.y) / 2.f - tb.position.y
    });
}

void GameOverState::spawnEmber() {
    const float W = float(WIN_W);
    const float H = float(WIN_H);
    Ember e;
    float r = 2.f + float(std::rand() % 4);
    e.shape.setRadius(r);
    e.shape.setOrigin({ r, r });
    e.shape.setPosition({ float(std::rand() % int(W)), H + 5.f });
    uint8_t red   = uint8_t(200 + std::rand() % 55);
    uint8_t green = uint8_t(std::rand() % 100);
    e.shape.setFillColor(sf::Color(red, green, 0));
    e.velocity     = { float(std::rand() % 60 - 30), float(-(60 + std::rand() % 100)) };
    e.maxLifetime  = 1.5f + float(std::rand() % 20) / 10.f;
    e.lifetime     = 0.f;
    e.flickerTimer = 0.f;
    m_embers.push_back(e);
}

void GameOverState::update(float dt) {
    m_elapsed += dt;

    // ── Fade in ──────────────────────────────────────────────
    if (m_fadeAlpha > 0.f)
        m_fadeAlpha = std::max(0.f, m_fadeAlpha - dt * 280.f);

    // ── Slide in panel (depuis le bas) ───────────────────────
    if (m_sliding) {
        m_panelY += (m_panelTargetY - m_panelY) * std::min(dt * 8.f, 1.f);
        if (std::abs(m_panelY - m_panelTargetY) < 0.5f) {
            m_panelY  = m_panelTargetY;
            m_sliding = false;
        }
        m_panel.setPosition({ m_panel.getPosition().x, m_panelY });
    }

    const float px = m_panel.getPosition().x;
    const float py = m_panelY;
    const float pw = m_panel.getSize().x;
    const float ph = m_panel.getSize().y;

    // ── Titre pulsant rouge ──────────────────────────────────
    if (m_titleText) {
        sf::FloatRect tb = m_titleText->getLocalBounds();
        float pulse = 1.f + 0.04f * std::sin(m_elapsed * 4.f);
        m_titleText->setScale({ pulse, pulse });
        m_titleText->setPosition({
            px + (pw - tb.size.x * pulse) / 2.f - tb.position.x,
            py + 18.f
        });
        uint8_t r = uint8_t(180 + 75 * std::abs(std::sin(m_elapsed * 3.f)));
        m_titleText->setFillColor(sf::Color(r, 20, 20));
    }

    // ── Compteurs animés ─────────────────────────────────────
    if (!m_countDone) {
        float speed = 60.f;
        m_scoreAnim = std::min(m_scoreAnim + float(m_score) * dt * speed / 100.f, float(m_score));
        m_killsAnim = std::min(m_killsAnim + float(m_kills) * dt * speed / 100.f, float(m_kills));
        m_waveAnim  = std::min(m_waveAnim  + float(m_wave)  * dt * speed / 100.f, float(m_wave));
        if (m_scoreAnim >= float(m_score) &&
            m_killsAnim >= float(m_kills) &&
            m_waveAnim  >= float(m_wave))
            m_countDone = true;
    }

    if (m_scoreLabel) m_scoreLabel->setString("Score  :  " + std::to_string(int(m_scoreAnim)));
    if (m_killsLabel) m_killsLabel->setString("Kills   :  " + std::to_string(int(m_killsAnim)));
    if (m_waveLabel)  m_waveLabel->setString ("Wave :  " + std::to_string(int(m_waveAnim)));

    if (m_scoreLabel) m_scoreLabel->setPosition({ px + 40.f, py + 110.f });
    if (m_killsLabel) m_killsLabel->setPosition({ px + 40.f, py + 148.f });
    if (m_waveLabel)  m_waveLabel->setPosition ({ px + 40.f, py + 186.f });

    // ── Boutons ──────────────────────────────────────────────
    float btnY   = py + ph - 66.f;
    float gap    = 20.f;
    float totalW = 180.f * 2 + gap;
    float btnX1  = px + (pw - totalW) / 2.f;
    float btnX2  = btnX1 + 180.f + gap;

    m_replayBtn.setPosition({ btnX1, btnY });
    m_menuBtn.setPosition  ({ btnX2, btnY });

    sf::Vector2f mouse = m_window.mapPixelToCoords(
        sf::Mouse::getPosition(m_window), m_window.getView());

    m_replayHover = m_replayBtn.getGlobalBounds().contains(mouse);
    m_menuHover   = m_menuBtn.getGlobalBounds().contains(mouse);

    m_replayBtn.setFillColor(m_replayHover ? sf::Color(130, 30, 30) : sf::Color(80, 20, 20));
    m_menuBtn.setFillColor  (m_menuHover   ? sf::Color(70,  70, 100) : sf::Color(40, 40, 60));

    if (m_replayLabel) centerText(*m_replayLabel, m_replayBtn.getGlobalBounds());
    if (m_menuLabel)   centerText(*m_menuLabel,   m_menuBtn.getGlobalBounds());

    // ── Braises ──────────────────────────────────────────────
    m_emberTimer += dt;
    if (m_emberTimer >= 0.04f) {
        m_emberTimer = 0.f;
        for (int i = 0; i < 4; ++i) spawnEmber();
    }

    for (auto& e : m_embers) {
        e.lifetime     += dt;
        e.flickerTimer += dt;
        e.shape.move(e.velocity * dt);
        // turbulence légère
        e.velocity.x += float(std::rand() % 20 - 10) * dt;
        // fondu
        float ratio = 1.f - e.lifetime / e.maxLifetime;
        sf::Color col = e.shape.getFillColor();
        col.a = uint8_t(255.f * ratio);
        e.shape.setFillColor(col);
    }

    m_embers.erase(
        std::remove_if(m_embers.begin(), m_embers.end(),
            [](const Ember& e) { return e.lifetime >= e.maxLifetime; }),
        m_embers.end()
    );
}

void GameOverState::handleEvents(const sf::Event& event) {
    if (const auto* mb = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = m_window.mapPixelToCoords(
                { mb->position.x, mb->position.y }, m_window.getView());
            if (m_replayBtn.getGlobalBounds().contains(pos)) { goToGame(); return; }
            if (m_menuBtn.getGlobalBounds().contains(pos))   { goToMenu(); return; }
        }
    }
    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Enter)  goToGame();
        if (kp->code == sf::Keyboard::Key::Escape) goToMenu();
    }
}

void GameOverState::render(sf::RenderWindow& window) {
    // Overlay sombre rouge
    sf::RectangleShape overlay{sf::Vector2f(float(WIN_W), float(WIN_H))};
    overlay.setFillColor(sf::Color(40, 0, 0, 180));
    window.draw(overlay);

    // Braises
    for (auto& e : m_embers)
        window.draw(e.shape);

    // Panel
    window.draw(m_panel);

    // Titre
    if (m_titleText) window.draw(*m_titleText);

    // Stats
    if (m_scoreLabel) window.draw(*m_scoreLabel);
    if (m_killsLabel) window.draw(*m_killsLabel);
    if (m_waveLabel)  window.draw(*m_waveLabel);

    // Boutons
    window.draw(m_replayBtn);
    window.draw(m_menuBtn);
    if (m_replayLabel) window.draw(*m_replayLabel);
    if (m_menuLabel)   window.draw(*m_menuLabel);

    // Fade in
    if (m_fadeAlpha > 0.f) {
        sf::RectangleShape fade{sf::Vector2f(float(WIN_W), float(WIN_H))};
        fade.setFillColor(sf::Color(0, 0, 0, uint8_t(m_fadeAlpha)));
        window.draw(fade);
    }
}

void GameOverState::goToMenu() {
    Game::getInstance().getStateManager()
        .change(std::make_unique<MenuState>(m_window));
}

void GameOverState::goToGame() {
    Game::getInstance().getStateManager()
        .change(std::make_unique<GameState>(m_window));
}