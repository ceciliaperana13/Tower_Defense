#include "View/WaveView.hpp"
#include <iostream>

WaveView::WaveView(WaveManager& waveManager)
    : m_waveManager(waveManager)
    , m_text(m_font)
{
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[WaveView] Impossible de charger la police\n";

    m_text.setFont(m_font);
    m_text.setCharacterSize(20);
    m_text.setFillColor(sf::Color::White);
    m_text.setOutlineColor(sf::Color::Black);
    m_text.setOutlineThickness(2.f);
}

void WaveView::setPosition(sf::Vector2f pos) {
    m_text.setPosition(pos);
}

void WaveView::update() {
    int current = m_waveManager.getCurrentWaveId();
    int total   = m_waveManager.getTotalWaves();
    m_text.setString("Wave " + std::to_string(current) + " / " + std::to_string(total));
}

void WaveView::render(sf::RenderWindow& window) {
    window.draw(m_text);
}