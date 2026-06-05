#pragma once
#include "State/IState.hpp"
#include "View/Leaderboard.hpp"
#include "Controller/SaveController.hpp"

class LeaderboardState : public IState {
public:
    explicit LeaderboardState(sf::RenderWindow& window);

    void onEnter() override;
    void onExit()  override {}

    void handleEvents(const sf::Event& event) override;
    void update(float /*dt*/) override {}
    void render(sf::RenderWindow& window) override;

    bool isTransparent() const override { return true; } // overlay par-dessus MenuState

private:
    void popSelf();

    sf::RenderWindow& m_window;
    SaveController&   m_saveCtrl;
    Leaderboard       m_leaderboard;
};