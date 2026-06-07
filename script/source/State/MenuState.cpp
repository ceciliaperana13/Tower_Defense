#include "State/MenuState.hpp"
#include "State/IntroState.hpp"
#include "State/OptionsState.hpp"
#include "State/LeaderboardState.hpp"
#include "Controller/SoundManager.hpp"
#include "Game.hpp"
#include <iostream>

MenuState::MenuState(sf::RenderWindow& window)
    : m_window(window)
    , m_menu(window)
{
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[MenuState] Font introuvable\n";
}

void MenuState::onEnter() {
    SoundManager::getInstance().playMusic("menu");
}

void MenuState::onExit() {
    SoundManager::getInstance().stopMusic();
}

// Builds the mode-picker popup centered on screen.
void MenuState::openModePicker() {
    m_pickingMode = true;

    const float W  = float(WIN_W);
    const float H  = float(WIN_H);
    constexpr float PW = 380.f;
    constexpr float PH = 160.f;
    float px = (W - PW) / 2.f;
    float py = (H - PH) / 2.f;

    m_pickerPanel.setSize({ PW, PH });
    m_pickerPanel.setPosition({ px, py });
    m_pickerPanel.setFillColor(sf::Color(15, 12, 30, 245));
    m_pickerPanel.setOutlineColor(sf::Color(200, 160, 60));
    m_pickerPanel.setOutlineThickness(2.5f);

    m_pickerTitle.emplace(m_font, "Choose a game mode", 20u);
    m_pickerTitle->setFillColor(sf::Color(220, 200, 120));
    m_pickerTitle->setStyle(sf::Text::Bold);
    sf::FloatRect tb = m_pickerTitle->getLocalBounds();
    m_pickerTitle->setPosition({
        px + (PW - tb.size.x) / 2.f - tb.position.x,
        py + 14.f
    });

    constexpr float BW = 150.f;
    constexpr float BH =  48.f;
    float gap  = (PW - 2.f * BW) / 3.f;
    float btnY = py + PH - BH - 20.f;

    // Normal button
    m_normalBtn.setSize({ BW, BH });
    m_normalBtn.setPosition({ px + gap, btnY });
    m_normalBtn.setFillColor(sf::Color(35, 90, 35));
    m_normalBtn.setOutlineColor(sf::Color(140, 200, 100));
    m_normalBtn.setOutlineThickness(2.f);

    m_normalLabel.emplace(m_font, "Normal Mode", 17u);
    m_normalLabel->setFillColor(sf::Color::White);
    sf::FloatRect nl = m_normalLabel->getLocalBounds();
    m_normalLabel->setPosition({
        px + gap + (BW - nl.size.x) / 2.f - nl.position.x,
        btnY + (BH - nl.size.y) / 2.f - nl.position.y
    });

    // Demo button
    m_demoBtn.setSize({ BW, BH });
    m_demoBtn.setPosition({ px + 2.f * gap + BW, btnY });
    m_demoBtn.setFillColor(sf::Color(80, 40, 10));
    m_demoBtn.setOutlineColor(sf::Color(220, 160, 60));
    m_demoBtn.setOutlineThickness(2.f);

    m_demoLabel.emplace(m_font, "Victory Demo", 17u);
    m_demoLabel->setFillColor(sf::Color::White);
    sf::FloatRect dl = m_demoLabel->getLocalBounds();
    m_demoLabel->setPosition({
        px + 2.f * gap + BW + (BW - dl.size.x) / 2.f - dl.position.x,
        btnY + (BH - dl.size.y) / 2.f - dl.position.y
    });
}

void MenuState::drawModePicker() {
    m_window.draw(m_pickerPanel);
    if (m_pickerTitle) m_window.draw(*m_pickerTitle);
    m_window.draw(m_normalBtn);
    if (m_normalLabel) m_window.draw(*m_normalLabel);
    m_window.draw(m_demoBtn);
    if (m_demoLabel) m_window.draw(*m_demoLabel);
}

void MenuState::handleModePickClick(sf::Vector2f pos) {
    auto& sm  = Game::getInstance().getStateManager();
    auto& win = Game::getInstance().getWindow();

    if (m_normalBtn.getGlobalBounds().contains(pos)) {
        m_pickingMode = false;
        sm.change(std::make_unique<IntroState>(win, false));
        return;
    }
    if (m_demoBtn.getGlobalBounds().contains(pos)) {
        m_pickingMode = false;
        sm.change(std::make_unique<IntroState>(win, true));
        return;
    }
    // Click outside panel → close picker
    if (!m_pickerPanel.getGlobalBounds().contains(pos))
        m_pickingMode = false;
}

void MenuState::handleEvents(const sf::Event& event) {
    m_mousePos = m_window.mapPixelToCoords(
        sf::Mouse::getPosition(m_window), m_window.getView());

    if (event.is<sf::Event::Closed>()) {
        m_window.close();
        return;
    }

    if (const auto* mb = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = m_window.mapPixelToCoords(
                { mb->position.x, mb->position.y }, m_window.getView());

            if (m_pickingMode) {
                handleModePickClick(pos);
                return;
            }

            for (auto& btn : m_menu.getButtons()) {
                if (btn.contains(pos)) {
                    handleAction(btn.getAction());
                    return;
                }
            }
            if (m_menu.settingsSpriteContains(pos)) {
                handleAction(MenuAction::Settings);
                return;
            }
        }
    }

    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape) {
            if (m_pickingMode)
                m_pickingMode = false;
            else
                handleAction(MenuAction::Exit);
        }
    }
}

void MenuState::handleAction(MenuAction action) {
    auto& sm  = Game::getInstance().getStateManager();
    auto& win = Game::getInstance().getWindow();

    switch (action) {
        case MenuAction::NewGame:
            openModePicker();      // show the two-choice popup instead of going directly
            break;
        case MenuAction::Settings:
            sm.push(std::make_unique<OptionsState>(win));
            break;
        case MenuAction::Scoreboard:
            sm.push(std::make_unique<LeaderboardState>(win));
            break;
        case MenuAction::Exit:
            win.close();
            break;
        default: break;
    }
}

void MenuState::update(float dt) {
    m_menu.update(m_mousePos, dt);
}

void MenuState::render(sf::RenderWindow& window) {
    m_menu.render();
    if (m_pickingMode)
        drawModePicker();
}