#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "Model/Wavemanager.hpp"

// Displays a "Next wave in Xs" countdown on a button_empty.png background.
// Hidden when not waiting between waves.
class TimerView : public sf::Drawable {
public:
    explicit TimerView(WaveManager& waveManager);

    bool load(const std::string& texturePath,
              const std::string& fontPath);

    void setPosition(const sf::Vector2f& position);
    void setScale(const sf::Vector2f& scale);
    void update();

private:
    void draw(sf::RenderTarget& target,
              sf::RenderStates  states) const override;

    WaveManager& m_waveManager;

    sf::Texture               m_texture;
    std::optional<sf::Sprite> m_sprite;

    sf::Font                  m_font;
    std::optional<sf::Text>   m_text;

    bool m_visible { false };
};