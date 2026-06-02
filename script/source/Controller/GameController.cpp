#include "GameController.hpp"
#include <iostream>

GameController::GameController(sf::RenderWindow& window)
    : m_window(window)
{
}

void GameController::startGame() {
    std::cout << "Game started\n";
}

void GameController::update(float dt) {
    // vide pour l'instant
}

void GameController::render() {
    // vide pour l'instant
}

void GameController::onInput(const sf::Event& e) {
    // vide pour l'instant
}
