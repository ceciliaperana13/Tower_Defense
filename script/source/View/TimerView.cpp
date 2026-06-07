#include "View/TimerView.hpp"
#include <iostream>
#include <cmath>

TimerView::TimerView(WaveManager& waveManager)
    : m_waveManager(waveManager)
{}

bool TimerView::load(const std::string& texturePath,
                     const std::string& fontPath)
{
    if (!m_texture.loadFromFile(texturePath)) {
        std::cerr << "[TimerView] Texture introuvable : " << texturePath << "\n";
        return false;
    }
    m_sprite.emplace(m_texture);

    if (!m_font.openFromFile(fontPath)) {
        std::cerr << "[TimerView] Police introuvable : " << fontPath << "\n";
        return false;
    }

    m_text.emplace(m_font, "", 16u);
    m_text->setFillColor(sf::Color(255, 210, 60));
    m_text->setStyle(sf::Text::Bold);

    return true;
}

void TimerView::setPosition(const sf::Vector2f& position) {
    if (m_sprite) m_sprite->setPosition(position);
    // Text is repositioned in update() after string changes
}

void TimerView::setScale(const sf::Vector2f& scale) {
    if (m_sprite) m_sprite->setScale(scale);
}

// Centers text over the sprite background
static void centerOn(sf::Text& text, const sf::Sprite& sprite) {
    sf::FloatRect sb = sprite.getGlobalBounds();
    sf::FloatRect tb = text.getLocalBounds();
    text.setPosition({
        sb.position.x + (sb.size.x - tb.size.x) / 2.f - tb.position.x,
        sb.position.y + (sb.size.y - tb.size.y) / 2.f - tb.position.y
    });
}

void TimerView::update() {
    if (!m_text || !m_sprite) return;

    m_visible = m_waveManager.isWaitingNextWave();

    if (!m_visible) return;

    int secs = static_cast<int>(std::ceil(m_waveManager.getNextWaveCountdown()));
    std::string label = "Next wave in " + std::to_string(secs) + "s";
    m_text->setString(label);

    // Pulse color: gold → orange as countdown approaches zero
    float t = m_waveManager.getNextWaveCountdown() / 10.f; // 0..1
    uint8_t r = 255;
    uint8_t g = static_cast<uint8_t>(140 + 70 * t);  // 140..210
    m_text->setFillColor(sf::Color(r, g, 40));

    centerOn(*m_text, *m_sprite);
}

void TimerView::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!m_visible) return;
    if (m_sprite) target.draw(*m_sprite, states);
    if (m_text)   target.draw(*m_text,   states);
}