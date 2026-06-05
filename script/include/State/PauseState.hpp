#pragma once
#include "State/IState.hpp"
#include <SFML/Graphics.hpp>
#include <optional>

class PauseState : public IState {
public:
    explicit PauseState(sf::RenderWindow& window);

    void onEnter() override;
    void onExit()  override;

    void handleEvents(const sf::Event& event) override;
    void update(float dt) override {}
    void render(sf::RenderWindow& window) override;

    bool isTransparent() const override { return true; }

private:
    void buildUI();
    void popSelf();

    sf::RenderWindow&  m_window;
    sf::Font           m_font;

    sf::RectangleShape      m_panel;
    sf::RectangleShape      m_resumeBtn;
    sf::RectangleShape      m_menuBtn;

    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_resumeLabel;
    std::optional<sf::Text> m_menuLabel;
};