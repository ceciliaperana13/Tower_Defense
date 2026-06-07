#include "State/OptionsState.hpp"
#include "Game.hpp"

OptionsState::OptionsState(sf::RenderWindow& window)
    : m_window(window)
    , m_settingsMenu(window, Game::getInstance().getSettings())
{}

void OptionsState::handleEvents(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        m_window.close();
        return;
    }

    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape) {
            popSelf();
            return;
        }
    }

    auto view = m_window.getView();

    if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = m_window.mapPixelToCoords(
                { mb->position.x, mb->position.y }, view);

            // Bouton fermer
            if (m_settingsMenu.closeBtnContains(pos)) {
                popSelf();
                return;
            }

            m_settingsMenu.handleClick(pos);
        }
    }

    if (const auto* mb = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mb->button == sf::Mouse::Button::Left)
            m_settingsMenu.stopDragging();
    }
}

void OptionsState::update(float dt) {
    m_mousePos = m_window.mapPixelToCoords(
        sf::Mouse::getPosition(m_window), m_window.getView());
    m_settingsMenu.update(m_mousePos);
}

void OptionsState::render(sf::RenderWindow& window) {
    m_settingsMenu.render(window);
}

void OptionsState::popSelf() {
    Game::getInstance().getStateManager().pop();
}