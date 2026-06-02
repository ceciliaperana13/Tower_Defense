#pragma once
#include <SFML/Graphics.hpp>
#include "Map.hpp"

class GameView {
public:
    GameView(sf::RenderWindow& window, Map& map)
        : m_window(window), m_map(map) {}

    void update(float dt) {}
    void render() {}

private:
    sf::RenderWindow& m_window;
    Map& m_map;
};
