#include "Leaderboard.hpp"
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

// ─── loadAssets 
void Leaderboard::loadAssets() {
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        throw std::runtime_error("Leaderboard: police introuvable.");
}

// ─── buildUI 
void Leaderboard::buildUI() {
    const float winW = m_window.getView().getSize().x;
    const float winH = m_window.getView().getSize().y;

    m_panelW = 620.f;
    m_panelH = 560.f;
    m_panelX = (winW - m_panelW) / 2.f;
    m_panelY = (winH - m_panelH) / 2.f;

    // Panneau principal
    m_panel.setSize({ m_panelW, m_panelH });
    m_panel.setPosition({ m_panelX, m_panelY });
    m_panel.setFillColor(sf::Color(15, 15, 25, 245));
    m_panel.setOutlineColor(sf::Color(180, 140, 60));
    m_panel.setOutlineThickness(2.f);

    // Titre
    m_titleText.emplace(m_font, "Leaderboard", 34u);
    m_titleText->setFillColor(sf::Color(220, 180, 80));
    m_titleText->setStyle(sf::Text::Bold);
    sf::FloatRect tb = m_titleText->getLocalBounds();
    m_titleText->setPosition({
        m_panelX + (m_panelW - tb.size.x) / 2.f - tb.position.x,
        m_panelY + 18.f
    });

    // Bouton fermer
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

// ─── handleEvents 
void Leaderboard::handleEvents() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {

        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            m_running = false;
            return;
        }

        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Escape) {
                m_running = false;
                return;
            }
        }

        if (const auto* mb = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mb->button == sf::Mouse::Button::Left) {
                sf::Vector2f pos = m_window.mapPixelToCoords({ mb->position.x, mb->position.y }, m_window.getView());
                if (m_closeBtn.getGlobalBounds().contains(pos)) {
                    m_running = false;
                    return;
                }
            }
        }
    }
}

// ─── run 
void Leaderboard::run() {
    m_running = true;
    while (m_running && m_window.isOpen()) {
        handleEvents();
        render();
    }
}

// ─── render 
void Leaderboard::render() {
    // Overlay sombre
    sf::RectangleShape overlay({
        m_window.getView().getSize().x,
        m_window.getView().getSize().y
    });
    overlay.setFillColor(sf::Color(0, 0, 0, 170));
    m_window.draw(overlay);

    // Panneau
    m_window.draw(m_panel);

    // Titre
    if (m_titleText) m_window.draw(*m_titleText);

    // Séparateur titre
    sf::RectangleShape sep({ m_panelW - 40.f, 1.f });
    sep.setPosition({ m_panelX + 20.f, m_panelY + 68.f });
    sep.setFillColor(sf::Color(180, 140, 60, 180));
    m_window.draw(sep);

    // Bouton fermer
    m_window.draw(m_closeBtn);
    if (m_closeLabel) m_window.draw(*m_closeLabel);

    // ── En-tête colonnes ────────────────────────────────────────
    const float colRank  = m_panelX + 30.f;
    const float colName  = m_panelX + 80.f;
    const float colScore = m_panelX + 340.f;
    const float colWave  = m_panelX + 450.f;
    const float colDate  = m_panelX + 510.f;
    const float headerY  = m_panelY + 82.f;

    auto makeText = [&](const std::string& str, float x, float y,
                        unsigned size, sf::Color color) {
        sf::Text t(m_font, str, size);
        t.setFillColor(color);
        t.setPosition({ x, y });
        return t;
    };

    sf::Color gold(220, 180, 80);
    sf::Color grey(160, 160, 160);

    m_window.draw(makeText("#",       colRank,  headerY, 16u, gold));
    m_window.draw(makeText("Joueur",  colName,  headerY, 16u, gold));
    m_window.draw(makeText("Score",   colScore, headerY, 16u, gold));
    m_window.draw(makeText("Vague",   colWave,  headerY, 16u, gold));
    m_window.draw(makeText("Date",    colDate,  headerY, 16u, gold));

    // Séparateur header
    sf::RectangleShape sep2({ m_panelW - 40.f, 1.f });
    sep2.setPosition({ m_panelX + 20.f, m_panelY + 108.f });
    sep2.setFillColor(sf::Color(180, 140, 60, 80));
    m_window.draw(sep2);

    // ── Lignes scores
    if (m_scores.empty()) {
        sf::Text empty(m_font, "Aucun score enregistre.", 20u);
        empty.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect eb = empty.getLocalBounds();
        empty.setPosition({
            m_panelX + (m_panelW - eb.size.x) / 2.f - eb.position.x,
            m_panelY + m_panelH / 2.f
        });
        m_window.draw(empty);
    } else {
        for (std::size_t i = 0; i < m_scores.size(); ++i) {
            const ScoreData& s = m_scores[i];
            const float rowY   = m_panelY + 118.f + static_cast<float>(i) * 42.f;

            // Fond alterné
            if (i % 2 == 0) {
                sf::RectangleShape row({ m_panelW - 40.f, 38.f });
                row.setPosition({ m_panelX + 20.f, rowY });
                row.setFillColor(sf::Color(255, 255, 255, 10));
                m_window.draw(row);
            }

            // Médaille top 3
            sf::Color rankColor = grey;
            std::string rankStr = std::to_string(i + 1);
            if      (i == 0) rankColor = sf::Color(255, 215,   0);   // or
            else if (i == 1) rankColor = sf::Color(192, 192, 192);   // argent
            else if (i == 2) rankColor = sf::Color(205, 127,  50);   // bronze

            m_window.draw(makeText(rankStr,               colRank,  rowY + 8.f, 17u, rankColor));
            m_window.draw(makeText(s.playerName,          colName,  rowY + 8.f, 17u, sf::Color::White));
            m_window.draw(makeText(std::to_string(s.score), colScore, rowY + 8.f, 17u, sf::Color(100, 220, 100)));
            m_window.draw(makeText(std::to_string(s.wave),  colWave,  rowY + 8.f, 17u, grey));
            m_window.draw(makeText(s.date,                colDate,  rowY + 8.f, 14u, grey));
        }
    }

    // Séparateur bas
    sf::RectangleShape sep3({ m_panelW - 40.f, 1.f });
    sep3.setPosition({ m_panelX + 20.f, m_panelY + m_panelH - 50.f });
    sep3.setFillColor(sf::Color(180, 140, 60, 60));
    m_window.draw(sep3);

    // Hint clavier
    sf::Text hint(m_font, "Echap pour fermer", 14u);
    hint.setFillColor(sf::Color(120, 120, 120));
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setPosition({
        m_panelX + (m_panelW - hb.size.x) / 2.f - hb.position.x,
        m_panelY + m_panelH - 34.f
    });
    m_window.draw(hint);

    m_window.display();
}