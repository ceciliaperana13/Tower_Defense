#include "State/PauseState.hpp"
#include "State/MenuState.hpp"
#include "Controller/SoundManager.hpp"
#include "Game.hpp"
#include <iostream>

PauseState::PauseState(sf::RenderWindow& window)
    : m_window(window)
{
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[PauseState] Font not found.\n";
    buildUI();
}

void PauseState::buildUI() {
    const float winW = float(WIN_W);
    const float winH = float(WIN_H);

    float pw = 320.f, ph = 220.f;
    float px = (winW - pw) / 2.f, py = (winH - ph) / 2.f;

    m_panel.setSize({ pw, ph });
    m_panel.setPosition({ px, py });
    m_panel.setFillColor(sf::Color(15, 15, 25, 230));
    m_panel.setOutlineColor(sf::Color(180, 140, 60));
    m_panel.setOutlineThickness(2.f);

    m_titleText.emplace(m_font, "PAUSE", 36u);
    m_titleText->setFillColor(sf::Color(220, 180, 80));
    m_titleText->setStyle(sf::Text::Bold);
    sf::FloatRect tb = m_titleText->getLocalBounds();
    m_titleText->setPosition({
        px + (pw - tb.size.x) / 2.f - tb.position.x,
        py + 20.f
    });

    // Bouton Reprendre
    m_resumeBtn.setSize({ 200.f, 50.f });
    m_resumeBtn.setPosition({ px + (pw - 200.f) / 2.f, py + 90.f });
    m_resumeBtn.setFillColor(sf::Color(45, 45, 55));
    m_resumeBtn.setOutlineColor(sf::Color(180, 140, 60));
    m_resumeBtn.setOutlineThickness(1.5f);

    m_resumeLabel.emplace(m_font, "Resume", 20u);
    m_resumeLabel->setFillColor(sf::Color::White);
    sf::FloatRect rb = m_resumeLabel->getLocalBounds();
    m_resumeLabel->setPosition({
        m_resumeBtn.getPosition().x + (200.f - rb.size.x) / 2.f - rb.position.x,
        m_resumeBtn.getPosition().y + (50.f  - rb.size.y) / 2.f - rb.position.y
    });

    // Bouton Menu principal
    m_menuBtn.setSize({ 200.f, 50.f });
    m_menuBtn.setPosition({ px + (pw - 200.f) / 2.f, py + 152.f });
    m_menuBtn.setFillColor(sf::Color(45, 45, 55));
    m_menuBtn.setOutlineColor(sf::Color(180, 140, 60));
    m_menuBtn.setOutlineThickness(1.5f);

    m_menuLabel.emplace(m_font, "Main Menu", 20u);
    m_menuLabel->setFillColor(sf::Color::White);
    sf::FloatRect mb2 = m_menuLabel->getLocalBounds();
    m_menuLabel->setPosition({
        m_menuBtn.getPosition().x + (200.f - mb2.size.x) / 2.f - mb2.position.x,
        m_menuBtn.getPosition().y + (50.f  - mb2.size.y) / 2.f - mb2.position.y
    });
}

void PauseState::onEnter() {
    SoundManager::getInstance().pauseMusic();
}

void PauseState::onExit() {
    SoundManager::getInstance().resumeMusic();
}

void PauseState::handleEvents(const sf::Event& event) {
    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape) { popSelf(); return; }
    }

    if (const auto* mb = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = m_window.mapPixelToCoords(
                { mb->position.x, mb->position.y }, m_window.getView());

            if (m_resumeBtn.getGlobalBounds().contains(pos)) {
                popSelf();
                return;
            }
            if (m_menuBtn.getGlobalBounds().contains(pos)) {
                Game::getInstance().getStateManager()
                    .change(std::make_unique<MenuState>(m_window));
            }
        }
    }
}

void PauseState::render(sf::RenderWindow& window) {
    // Overlay semi-transparent par-dessus le jeu
    sf::RectangleShape overlay{sf::Vector2f(float(WIN_W), float(WIN_H))};
    overlay.setFillColor(sf::Color(0, 0, 0, 120));
    window.draw(overlay);

    window.draw(m_panel);
    if (m_titleText)  window.draw(*m_titleText);
    window.draw(m_resumeBtn);
    if (m_resumeLabel) window.draw(*m_resumeLabel);
    window.draw(m_menuBtn);
    if (m_menuLabel)   window.draw(*m_menuLabel);
}

void PauseState::popSelf() {
    Game::getInstance().getStateManager().pop();
}