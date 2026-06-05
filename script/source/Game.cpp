#include "Game.hpp"
#include "State/MenuState.hpp"
#include <iostream>
#include "State/SplashState.hpp"

Game::Game() {
    openWindowed();
}

sf::View Game::makeWindowedView() const {
    return sf::View(sf::FloatRect(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(float(WIN_W), float(WIN_H))
    ));
}

void Game::openWindowed() {
    m_window.create(
        sf::VideoMode(sf::Vector2u(WIN_W, WIN_H)),
        "Defend the Castle",
        sf::State::Windowed
    );
    m_window.setFramerateLimit(60);
    m_window.setView(makeWindowedView());
    m_fullscreen = false;
}
// plein écran
void Game::openFullscreen() {
    auto desk = sf::VideoMode::getDesktopMode();
    m_window.create(
        sf::VideoMode(sf::Vector2u(desk.size.x, desk.size.y)),
        "Defend the Castle",
        sf::State::Fullscreen
    );
    m_window.setFramerateLimit(60);
    m_window.setView(makeWindowedView());
    m_fullscreen = true;
}

void Game::toggleFullscreen() {
    if (m_fullscreen) openWindowed();
    else              openFullscreen();
}

void Game::run() {
    m_stateManager.change(std::make_unique<MenuState>(m_window));

    while (m_window.isOpen() && !m_stateManager.isEmpty()) {
        float dt = m_clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    while (const auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            return;
        }
        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::F11) {
                toggleFullscreen();
                return;
            }
        }
        m_stateManager.handleEvents(*event);
    }
}

void Game::update(float dt) {
    m_stateManager.update(dt);
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    m_stateManager.render(m_window);
    m_window.display();
}