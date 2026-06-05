#pragma once
#include "State/IState.hpp"
#include <SFML/Graphics.hpp>
#include <optional>

class IntroState : public IState {
public:
    explicit IntroState(sf::RenderWindow& window);

    void onEnter() override {}
    void onExit()  override {}

    void handleEvents(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void goToGame();

    sf::RenderWindow& m_window;

    // Textures
    sf::Texture m_bgTex;
    sf::Texture m_castleTex;
    sf::Texture m_ogreTex[2];

    // Sprites
    std::optional<sf::Sprite> m_bgSprite;
    std::optional<sf::Sprite> m_castleSprite;
    std::optional<sf::Sprite> m_ogreSprite;

    // Textes
    sf::Font                m_font;
    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_skipText;
    std::optional<sf::Text> m_barLabel;

    // Barre de chargement
    sf::RectangleShape m_barBg;
    sf::RectangleShape m_barFill;

    // Timing
    static constexpr float DURATION = 8.f;
    float m_elapsed = 0.f;

    // Animation ogre
    float m_animTimer = 0.f;
    int   m_animFrame = 0;
    static constexpr float ANIM_SPEED = 0.30f;

    // Mouvement ogre
    float m_ogreX    = 0.f;
    float m_ogreSpeed = 0.f;

    // Shake château
    sf::Vector2f m_castleBasePos;
    bool  m_shaking    = false;
    float m_shakeTimer = 0.f;

    // Flash impact
    bool  m_flashActive = false;
    float m_flashTimer  = 0.f;

    // Fade out final
    float m_fadeAlpha = 0.f;
    bool  m_fadingOut = false;
};