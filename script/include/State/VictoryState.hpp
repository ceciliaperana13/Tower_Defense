#pragma once
#include "State/IState.hpp"
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

struct Confetti {
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float rotSpeed;
    float lifetime;
    float maxLifetime;
};

class VictoryState : public IState {
public:
    explicit VictoryState(sf::RenderWindow& window,
                          int score, int kills, int wave);

    void onEnter() override;
    void onExit()  override {}

    void handleEvents(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void buildUI();
    void spawnConfetti();
    void goToMenu();
    void goToGame();

    sf::RenderWindow& m_window;
    sf::Font          m_font;
    int m_score, m_kills, m_wave;

    // Panel - animation slide in
    sf::RectangleShape      m_panel;
    float                   m_panelY       { 0.f };
    float                   m_panelTargetY { 0.f };
    bool                    m_sliding      { true };

    // Fade in
    float m_fadeAlpha { 255.f };

    // Textes
    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_scoreLabel;
    std::optional<sf::Text> m_killsLabel;
    std::optional<sf::Text> m_waveLabel;

    // Compteurs animés
    float m_scoreAnim { 0.f };
    float m_killsAnim { 0.f };
    float m_waveAnim  { 0.f };
    bool  m_countDone { false };

    // Boutons
    sf::RectangleShape      m_replayBtn;
    sf::RectangleShape      m_menuBtn;
    std::optional<sf::Text> m_replayLabel;
    std::optional<sf::Text> m_menuLabel;
    bool m_replayHover { false };
    bool m_menuHover   { false };

    // Confettis
    std::vector<Confetti> m_confetti;
    float m_confettiTimer { 0.f };

    // Elapsed
    float m_elapsed { 0.f };
};