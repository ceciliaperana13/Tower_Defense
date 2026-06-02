#include "TimerView.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

// ─── Constructeur 
TimerView::TimerView(CountdownTimer& timer)
    : m_timer(timer)
{}

// ─── load ─
bool TimerView::load(const std::string& texturePath,
                     const std::string& fontPath)
{
    // Sprite (icône timer)
    if (!m_texture.loadFromFile(texturePath)) {
        std::cerr << "[TimerView] Texture introuvable : " << texturePath << "\n";
        return false;
    }
    m_sprite.emplace(m_texture);

    // Texte
    if (!m_font.openFromFile(fontPath)) {
        std::cerr << "[TimerView] Police introuvable : " << fontPath << "\n";
        return false;
    }
    m_text.emplace(m_font, "00:00", 28u);
    m_text->setFillColor(sf::Color::White);
    m_text->setStyle(sf::Text::Bold);

    return true;
}

// ─── setPosition 
void TimerView::setPosition(const sf::Vector2f& position) {
    if (m_sprite.has_value())
        m_sprite->setPosition(position);

    // Texte décalé à droite de l'icône
    if (m_text.has_value()) {
        float iconW = m_sprite.has_value()
            ? m_sprite->getGlobalBounds().size.x
            : 0.f;
        m_text->setPosition({ position.x + iconW + 8.f, position.y });
    }
}

// ─── setScale 
void TimerView::setScale(const sf::Vector2f& scale) {
    if (m_sprite.has_value())
        m_sprite->setScale(scale);
}

// ─── update 
void TimerView::update() {
    if (!m_text.has_value()) return;

    float r   = m_timer.remaining();
    int   min = static_cast<int>(r) / 60;
    int   sec = static_cast<int>(r) % 60;

    std::ostringstream ss;
    ss << std::setw(2) << std::setfill('0') << min << ":"
       << std::setw(2) << std::setfill('0') << sec;

    m_text->setString(ss.str());

    // Clignote en rouge quand il reste moins de 10 secondes
    if (r <= 10.f) {
        float alpha = (static_cast<int>(r * 4) % 2 == 0) ? 255.f : 100.f;
        m_text->setFillColor(sf::Color(255, 80, 80, static_cast<uint8_t>(alpha)));
    } else {
        m_text->setFillColor(sf::Color::White);
    }
}

// ─── draw 
void TimerView::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (m_sprite.has_value())
        target.draw(*m_sprite, states);
    if (m_text.has_value())
        target.draw(*m_text, states);
}