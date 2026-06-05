#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <string>
#include "Controller/SaveController.hpp"

class Leaderboard {
public:
    explicit Leaderboard(sf::RenderWindow& window, SaveController& saveCtrl);
    ~Leaderboard() = default;

    void run();

    // ← Ajouts pour LeaderboardState
    void reloadScores();
    bool closeBtnContains(sf::Vector2f pos) const;
    void render(sf::RenderWindow& window);

private:
    void loadAssets();
    void buildUI();
    void handleEvents();
    void render(); // interne pour run()

    sf::RenderWindow& m_window;
    SaveController&   m_saveCtrl;

    sf::Font                  m_font;
    std::optional<sf::Text>   m_titleText;

    sf::RectangleShape        m_panel;
    sf::RectangleShape        m_closeBtn;
    std::optional<sf::Text>   m_closeLabel;

    std::vector<ScoreData>    m_scores;

    bool  m_running { true };
    float m_panelX {}, m_panelY {}, m_panelW {}, m_panelH {};
};