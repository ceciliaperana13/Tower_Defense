#pragma once
#include <SFML/Graphics.hpp>
#include "../Model/WaveManager.hpp"

class WaveView {
public:
    WaveView(WaveManager& waveManager);

    void setPosition(sf::Vector2f pos);
    void update();
    void render(sf::RenderWindow& window);

private:
    WaveManager& m_waveManager;

    sf::Font m_font;
    sf::Text m_text;
};