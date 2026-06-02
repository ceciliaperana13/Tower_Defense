//GameController complet avec Map, GameView, SoundManager, Timer, GameState a gerer en fxml3
#pragma once 
#include <SFML/Graphics.hpp>

class GameController{
    public:
        GameController(sf::RenderWindow& window);
        ~GameController() = default;

        void run();

    private:
        sf::RenderWindow& m_window;
        bool m_running { true };

};
