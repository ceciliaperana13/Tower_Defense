#include "MainMenu.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(1080, 720), "Defend the Castle");

    while (window.isOpen()) {
        MainMenu menu(window);
        MenuAction action = menu.run();

        if (action == MenuAction::NewGame)    { /* lancer GameController */ }
        else if (action == MenuAction::Scoreboard) { /* afficher scores  */ }
        else if (action == MenuAction::Settings)   { /* ouvrir Settings  */ }
        else { window.close(); }
    }
}