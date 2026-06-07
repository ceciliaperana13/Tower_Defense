#include "View/Leaderboard.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

Leaderboard::Leaderboard(sf::RenderWindow& window, SaveController& saveCtrl)
    : m_window(window), m_saveCtrl(saveCtrl)
{
    loadAssets();
    buildUI();
    m_scores = m_saveCtrl.loadScores();
}

void Leaderboard::loadAssets() {
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        throw std::runtime_error("Leaderboard: police introuvable.");
}

void Leaderboard::buildUI() {
    const float winW = m_window.getView().getSize().x;
    const float winH = m_window.getView().getSize().y;

    m_panelW = 720.f;
    m_panelH = 560.f;
    m_panelX = (winW - m_panelW) / 2.f;
    m_panelY = (winH - m_panelH) / 2.f;

    m_panel.setSize({ m_panelW, m_panelH });
    m_panel.setPosition({ m_panelX, m_panelY });
    m_panel.setFillColor(sf::Color(15, 15, 25, 245));
    m_panel.setOutlineColor(sf::Color(180, 140, 60));
    m_panel.setOutlineThickness(2.f);

    m_titleText.emplace(m_font, "Leaderboard", 34u);
    m_titleText->setFillColor(sf::Color(220, 180, 80));
    m_titleText->setStyle(sf::Text::Bold);
    sf::FloatRect tb = m_titleText->getLocalBounds();
    m_titleText->setPosition({
        m_panelX + (m_panelW - tb.size.x) / 2.f - tb.position.x,
        m_panelY + 18.f
    });

    m_closeBtn.setSize({ 30.f, 30.f });
    m_closeBtn.setPosition({ m_panelX + m_panelW - 40.f, m_panelY + 12.f });
    m_closeBtn.setFillColor(sf::Color(180, 40, 40));
    m_closeBtn.setOutlineColor(sf::Color(255, 100, 100));
    m_closeBtn.setOutlineThickness(1.f);

    m_closeLabel.emplace(m_font, "X", 18u);
    m_closeLabel->setFillColor(sf::Color::White);
    sf::FloatRect cb = m_closeLabel->getLocalBounds();
    m_closeLabel->setPosition({
        m_closeBtn.getPosition().x + (30.f - cb.size.x) / 2.f - cb.position.x,
        m_closeBtn.getPosition().y + (30.f - cb.size.y) / 2.f - cb.position.y
    });
}

// ─── API publique pour LeaderboardState
void Leaderboard::reloadScores() {
    m_scores = m_saveCtrl.loadScores();
}

bool Leaderboard::closeBtnContains(sf::Vector2f pos) const {
    return m_closeBtn.getGlobalBounds().contains(pos);
}

void Leaderboard::handleEvents() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { m_window.close(); m_running = false; return; }
        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Escape) { m_running = false; return; }
        }
        if (const auto* mb = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mb->button == sf::Mouse::Button::Left) {
                sf::Vector2f pos = m_window.mapPixelToCoords(
                    { mb->position.x, mb->position.y }, m_window.getView());
                if (closeBtnContains(pos)) { m_running = false; return; }
            }
        }
    }
}

void Leaderboard::run() {
    reloadScores();
    m_running = true;
    while (m_running && m_window.isOpen()) {
        handleEvents();
        render();
    }
}

// ─── render interne (pour run())
void Leaderboard::render() {
    render(m_window);
    m_window.display();
}

// ─── render publique (pour LeaderboardState) — sans display()
void Leaderboard::render(sf::RenderWindow& window) {
    sf::RectangleShape overlay({
        window.getView().getSize().x,
        window.getView().getSize().y
    });
    overlay.setFillColor(sf::Color(0, 0, 0, 170));
    window.draw(overlay);

    window.draw(m_panel);
    if (m_titleText) window.draw(*m_titleText);

    sf::RectangleShape sep({ m_panelW - 40.f, 1.f });
    sep.setPosition({ m_panelX + 20.f, m_panelY + 68.f });
    sep.setFillColor(sf::Color(180, 140, 60, 180));
    window.draw(sep);

    window.draw(m_closeBtn);
    if (m_closeLabel) window.draw(*m_closeLabel);

    const float colRank  = m_panelX + 22.f;
    const float colName  = m_panelX + 65.f;
    const float colKills = m_panelX + 300.f;
    const float colWave  = m_panelX + 390.f;
    const float colScore = m_panelX + 470.f;
    const float colDate  = m_panelX + 560.f;
    const float headerY  = m_panelY + 82.f;

    auto makeText = [&](const std::string& str, float x, float y,
                        unsigned size, sf::Color color) -> sf::Text {
        sf::Text t(m_font, str, size);
        t.setFillColor(color);
        t.setPosition({ x, y });
        return t;
    };

    const sf::Color gold  (220, 180,  80);
    const sf::Color grey  (160, 160, 160);
    const sf::Color green (100, 220, 100);
    const sf::Color cyan  ( 80, 200, 220);
    const sf::Color yellow(240, 220,  60);

    window.draw(makeText("#",      colRank,  headerY, 15u, gold));
    window.draw(makeText("Joueur", colName,  headerY, 15u, gold));
    window.draw(makeText("Kills",  colKills, headerY, 15u, gold));
    window.draw(makeText("Wave",  colWave,  headerY, 15u, gold));
    window.draw(makeText("Score",  colScore, headerY, 15u, gold));
    window.draw(makeText("Date",   colDate,  headerY, 15u, gold));

    sf::RectangleShape sep2({ m_panelW - 40.f, 1.f });
    sep2.setPosition({ m_panelX + 20.f, m_panelY + 108.f });
    sep2.setFillColor(sf::Color(180, 140, 60, 80));
    window.draw(sep2);

    if (m_scores.empty()) {
        sf::Text empty(m_font, "Aucun score enregistre.", 20u);
        empty.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect eb = empty.getLocalBounds();
        empty.setPosition({
            m_panelX + (m_panelW - eb.size.x) / 2.f - eb.position.x,
            m_panelY + m_panelH / 2.f
        });
        window.draw(empty);
    } else {
        for (std::size_t i = 0; i < m_scores.size(); ++i) {
            const ScoreData& s    = m_scores[i];
            const float      rowY = m_panelY + 118.f + float(i) * 42.f;

            if (i % 2 == 0) {
                sf::RectangleShape row({ m_panelW - 40.f, 38.f });
                row.setPosition({ m_panelX + 20.f, rowY });
                row.setFillColor(sf::Color(255, 255, 255, 10));
                window.draw(row);
            }

            sf::Color rankColor = grey;
            if      (i == 0) rankColor = sf::Color(255, 215,   0);
            else if (i == 1) rankColor = sf::Color(192, 192, 192);
            else if (i == 2) rankColor = sf::Color(205, 127,  50);

            const float ty = rowY + 8.f;
            window.draw(makeText(std::to_string(i + 1),        colRank,  ty, 16u, rankColor));
            window.draw(makeText(s.playerName,                 colName,  ty, 16u, sf::Color::White));
            window.draw(makeText(std::to_string(s.enemyCount), colKills, ty, 16u, cyan));
            window.draw(makeText("v" + std::to_string(s.wave), colWave,  ty, 16u, yellow));
            window.draw(makeText(std::to_string(s.score),      colScore, ty, 16u, green));
            window.draw(makeText(s.date,                       colDate,  ty, 13u, grey));
        }
    }

    sf::RectangleShape sep3({ m_panelW - 40.f, 1.f });
    sep3.setPosition({ m_panelX + 20.f, m_panelY + m_panelH - 50.f });
    sep3.setFillColor(sf::Color(180, 140, 60, 60));
    window.draw(sep3);

    sf::Text hint(m_font, "Echap pour fermer", 14u);
    hint.setFillColor(sf::Color(120, 120, 120));
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setPosition({
        m_panelX + (m_panelW - hb.size.x) / 2.f - hb.position.x,
        m_panelY + m_panelH - 34.f
    });
    window.draw(hint);
}