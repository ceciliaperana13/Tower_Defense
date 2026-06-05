#pragma once
#include <SFML/Graphics.hpp>
#include "State/StateManager.hpp"
#include "View/GameView.hpp"
#include "View/GameView.hpp"
#include "View/GameView.hpp"
#include "Controller/SaveController.hpp"
class Game {
public:
    static Game& getInstance() {
        static Game instance;
        return instance;
    }
    void run();
    sf::RenderWindow& getWindow()        { return m_window; }
    StateManager&     getStateManager()  { return m_stateManager; }
    SaveController&   getSaveController(){ return m_saveCtrl; }
    void toggleFullscreen();
    bool isFullscreen() const { return m_fullscreen; }
private:
    Game();
    Game(const Game&)            = delete;
    Game& operator=(const Game&) = delete;
    void processEvents();
    void update(float dt);
    void render();
    sf::View makeWindowedView() const;
    void     openWindowed();
    void     openFullscreen();
    sf::RenderWindow m_window;
    StateManager     m_stateManager;
    SaveController   m_saveCtrl;
    sf::Clock        m_clock;
    bool             m_fullscreen { false };
};