#include "State/MenuState.hpp"
#include "Controller/SoundManager.hpp"
#include "State/IntroState.hpp"
#include "State/OptionsState.hpp"
#include "State/LeaderboardState.hpp"
#include "Game.hpp"

MenuState::MenuState(sf::RenderWindow& window)
    : m_window(window)
    , m_menu(window)
{}

void MenuState::onEnter() {
    SoundManager::getInstance().playMusic("menu");
}

void MenuState::onExit() {
    SoundManager::getInstance().stopMusic();
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
        if (kp->code == sf::Keyboard::Key::Escape)
            handleAction(MenuAction::Exit);
    }
}

void MenuState::handleAction(MenuAction action) {
    auto& sm  = Game::getInstance().getStateManager();
    auto& win = Game::getInstance().getWindow();

    switch (action) {
        case MenuAction::NewGame:
            sm.change(std::make_unique<IntroState>(win));
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
}