#pragma once
#include <SFML/Graphics.hpp>

class GameController {
public:
    GameController(sf::RenderWindow& window);

    void startGame();
    void update(float dt);
    void render();
    void onInput(const sf::Event& e);

private:
    sf::RenderWindow& m_window;
};
