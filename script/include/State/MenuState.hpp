#pragma once
#include "State/IState.hpp"
#include "View/MainMenu.hpp"
#include <SFML/Graphics.hpp>
#include <optional>

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
    void handleModePickClick(sf::Vector2f pos);
    void openModePicker();
    void drawModePicker();

    sf::RenderWindow& m_window;
    MainMenu          m_menu;
    sf::Vector2f      m_mousePos;

    // Mode-picker popup (shown after clicking New Game)
    bool m_pickingMode { false };

    sf::Font               m_font;
    sf::RectangleShape     m_pickerPanel;
    sf::RectangleShape     m_normalBtn;
    sf::RectangleShape     m_demoBtn;
    std::optional<sf::Text> m_pickerTitle;
    std::optional<sf::Text> m_normalLabel;
    std::optional<sf::Text> m_demoLabel;
};