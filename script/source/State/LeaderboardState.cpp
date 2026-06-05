#include "State/LeaderboardState.hpp"
#include "Game.hpp"

LeaderboardState::LeaderboardState(sf::RenderWindow& window)
    : m_window(window)
    , m_saveCtrl(Game::getInstance().getSaveController())
    , m_leaderboard(window, m_saveCtrl)
{}

void LeaderboardState::onEnter() {
    // Recharge les scores à chaque ouverture
    m_leaderboard.reloadScores();
}

void LeaderboardState::handleEvents(const sf::Event& event) {
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

    if (const auto* mb = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = m_window.mapPixelToCoords(
                { mb->position.x, mb->position.y }, m_window.getView());
            if (m_leaderboard.closeBtnContains(pos))
                popSelf();
        }
    }
}

void LeaderboardState::render(sf::RenderWindow& window) {
    m_leaderboard.render(window);
}

void LeaderboardState::popSelf() {
    Game::getInstance().getStateManager().pop();
}