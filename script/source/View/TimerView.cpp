#include "TimerView.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

TimerView::TimerView(CountdownTimer& timer)
    : m_timer(timer)
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

    m_text.emplace(m_font, "00:00", 20u);
    m_text->setFillColor(sf::Color::White);
    m_text->setStyle(sf::Text::Bold);

    return true;
}

// ─── setPosition : texte centré sur le sprite ───────────────────
void TimerView::setPosition(const sf::Vector2f& position) {
    if (m_sprite.has_value())
        m_sprite->setPosition(position);

    if (m_text.has_value()) {
        // Centre le texte sur le sprite
        sf::FloatRect sprBounds = m_sprite.has_value()
            ? m_sprite->getGlobalBounds()
            : sf::FloatRect({ position.x, position.y }, { 80.f, 20.f });

        sf::FloatRect txtBounds = m_text->getLocalBounds();

        float tx = sprBounds.position.x + (sprBounds.size.x - txtBounds.size.x) / 2.f
                   - txtBounds.position.x;
        float ty = sprBounds.position.y + (sprBounds.size.y - txtBounds.size.y) / 2.f
                   - txtBounds.position.y;

        m_text->setPosition({ tx, ty });
    }
}

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

    // Recentre après changement de string
    if (m_sprite.has_value()) {
        sf::FloatRect sprBounds = m_sprite->getGlobalBounds();
        sf::FloatRect txtBounds = m_text->getLocalBounds();

        float tx = sprBounds.position.x + (sprBounds.size.x - txtBounds.size.x) / 2.f
                   - txtBounds.position.x;
        float ty = sprBounds.position.y + (sprBounds.size.y - txtBounds.size.y) / 2.f
                   - txtBounds.position.y;

        m_text->setPosition({ tx, ty });
    }

    // Clignote en rouge sous 10 secondes
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
//compteur de vague, affiché en haut de l'écran pendant le chargement de la vague suivante, puis caché pendant la vague. Il doit clignoter en rouge lorsque le temps restant est inférieur à 10 secondes.