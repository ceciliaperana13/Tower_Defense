#pragma once
#include <SFML/Graphics.hpp>

class EndGameManager {
public:
    enum class State {
        None,
        Victory,
        Defeat
    };

    EndGameManager();

    void triggerVictory();
    void triggerDefeat();

    bool hasEnded() const;
    State getState() const;

    void showPopup(sf::RenderWindow& window);

private:
    State state;
    sf::Font m_font;

    void drawPopup(sf::RenderWindow& window);
};
