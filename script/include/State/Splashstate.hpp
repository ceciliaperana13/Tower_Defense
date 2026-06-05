#pragma once
#include "State/IState.hpp"
#include <SFML/Graphics.hpp>
#include <optional>

class SplashState : public IState {
public:
    explicit SplashState(sf::RenderWindow& window);

    void onEnter() override;
    void onExit()  override {}

    void handleEvents(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void buildUI();
    void goToMenu();

    sf::RenderWindow& m_window;

    // Textures
    sf::Texture m_castleTex;
    sf::Texture m_ogreTex[2];   // ogre_boss1 / ogre_boss2
    sf::Texture m_bgTex;

    // Sprites
    std::optional<sf::Sprite> m_bgSprite;
    std::optional<sf::Sprite> m_castleSprite;
    std::optional<sf::Sprite> m_ogreSprite;

    // Barre de progression
    sf::RectangleShape m_barBg;
    sf::RectangleShape m_barFill;
    std::optional<sf::Text> m_barLabel;
    sf::Font m_font;

    // Titre cinématique
    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_skipText;

    // Timing
    static constexpr float DURATION = 20.f;
    float m_elapsed   = 0.f;

    // Animation ogre
    float m_animTimer = 0.f;
    int   m_animFrame = 0;
    static constexpr float ANIM_SPEED = 0.35f;

    // Mouvement ogre (marche vers le château)
    float m_ogreX     = 0.f;
    float m_ogreSpeed = 0.f;   // px/s, calculé à l'init

    // Shake du château quand l'ogre arrive
    float m_shakeTimer  = 0.f;
    bool  m_shaking     = false;
    sf::Vector2f m_castleBasePos;

    // Flash d'impact
    float m_flashTimer  = 0.f;
    bool  m_flashActive = false;
};