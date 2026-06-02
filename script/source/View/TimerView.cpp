// #include "TimerView.hpp"

// #include <iomanip>
// #include <sstream>

// TimerView::TimerView(CountdownTimer& timer)
//     : m_timer(timer)
// // Initialisation du texte en sfml3 pour éviter les problèmes de police non chargée{
//     m_text.setFont(sf::Font());
//     m_text.setString("00:00");

// }
// bool TimerView::load(
//     const std::string& texturePath,
//     const std::string& fontPath
// )
// {
//     if (!m_texture.loadFromFile(texturePath))
//         return false;

//     if (!m_font.openFromFile(fontPath))
//         return false;

//     m_sprite.setTexture(m_texture);

//     m_text.setFont(m_font);

//     return true;
// }

// void TimerView::setPosition(const sf::Vector2f& position)
// {
//     m_sprite.setPosition(position);
// }

// void TimerView::setScale(const sf::Vector2f& scale)
// {
//     m_sprite.setScale(scale);
// }

// void TimerView::update()
// {
//     int totalSeconds =
//         static_cast<int>(m_timer.remaining());

//     int minutes = totalSeconds / 60;
//     int seconds = totalSeconds % 60;

//     std::ostringstream oss;

//     oss << std::setw(2)
//         << std::setfill('0')
//         << minutes
//         << ":"
//         << std::setw(2)
//         << std::setfill('0')
//         << seconds;

//     m_text.setString(oss.str());

//     auto textBounds = m_text.getLocalBounds();

//     m_text.setOrigin({
//         textBounds.position.x + textBounds.size.x / 2.f,
//         textBounds.position.y + textBounds.size.y / 2.f
//     });

//     auto spriteBounds = m_sprite.getGlobalBounds();

//     m_text.setPosition({
//         spriteBounds.position.x + spriteBounds.size.x / 2.f,
//         spriteBounds.position.y + spriteBounds.size.y / 2.f
//     });
// }

// void TimerView::draw(
//     sf::RenderTarget& target,
//     sf::RenderStates states
// ) const
// {
//     target.draw(m_sprite, states);
//     target.draw(m_text, states);
// }