#pragma once
#include "State/IState.hpp"
#include "View/MainMenu.hpp"

class MenuState : public IState {
public:
    explicit MenuState(sf::RenderWindow& window);

    void onEnter()  override;
    void onExit()   override;

    void handleEvents(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    void handleAction(MenuAction action);

    sf::RenderWindow& m_window;
    MainMenu          m_menu;
    sf::Vector2f      m_mousePos;
};