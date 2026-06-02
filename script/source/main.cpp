#include "View/MainMenu.hpp"
#include "View/SettingsMenu.hpp"
#include "View/Leaderboard.hpp"
#include "Controller/SaveController.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1080u, 720u }), "Defend the Castle");
    window.setFramerateLimit(60);

    GameSettings   settings;
    SaveController saveCtrl;

    while (window.isOpen()) {
        MainMenu   menu(window);
        MenuAction action = menu.run();

        if (action == MenuAction::Exit || !window.isOpen())
            break;

        if (action == MenuAction::Settings) {
            SettingsMenu sm(window, settings);
            sm.run();
            continue;
        }

        if (action == MenuAction::Scoreboard) {
            Leaderboard lb(window, saveCtrl);
            lb.run();
            continue;
        }

        if (action == MenuAction::NewGame) {
            // TODO : lancer le jeu
            break;
        }
    }

    return 0;
}