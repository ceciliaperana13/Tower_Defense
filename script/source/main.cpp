#include "MainMenu.hpp"
#include "SettingsMenu.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1080u, 720u }), "Defend the Castle");
    window.setFramerateLimit(60);

    GameSettings settings;

    while (window.isOpen()) {
        MainMenu menu(window);
        MenuAction action = menu.run();

        if (action == MenuAction::Exit || !window.isOpen())
            break;

        if (action == MenuAction::Settings) {
            SettingsMenu sm(window, settings);
            sm.run();
            // Retourne au menu principal après fermeture des settings
            continue;
        }

        if (action == MenuAction::NewGame) {
            // TODO : lancer le jeu
            break;
        }

        if (action == MenuAction::Scoreboard) {
            // TODO : afficher le scoreboard
            break;
        }
    }

    return 0;
}