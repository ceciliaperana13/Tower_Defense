#pragma once
#include "State/IState.hpp"
#include "View/SettingsMenu.hpp"

class OptionsState : public IState {
public:
    explicit OptionsState(sf::RenderWindow& window);

    void onEnter() override {}
    void onExit()  override {}

    void handleEvents(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    bool isTransparent() const override { return true; } // overlay par-dessus MenuState

private:
    void handleAction(sf::Vector2f pos, const sf::Event& event);
    void popSelf();

    sf::RenderWindow& m_window;
    GameSettings      m_settings;
    SettingsMenu      m_settingsMenu;
    sf::Vector2f      m_mousePos;
};