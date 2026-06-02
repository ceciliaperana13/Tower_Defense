#pragma once

#include <SFML/Graphics.hpp>
#include "CountdownTimer.hpp"

class TimerView : public sf::Drawable
{
public:
    explicit TimerView(CountdownTimer& timer);

    bool load(
        const std::string& texturePath,
        const std::string& fontPath
    );

    void setPosition(const sf::Vector2f& position);
    void setScale(const sf::Vector2f& scale);

    void update();

private:
    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states
    ) const override;

private:
    CountdownTimer& m_timer;

    sf::Texture m_texture;
    sf::Sprite m_sprite;

    sf::Font m_font;
    sf::Text m_text;
};