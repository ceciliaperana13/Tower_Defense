#include "MainMenu.hpp"

int main() {
    // SFML 3 
    sf::RenderWindow window(sf::VideoMode({ 1080u, 720u }), "Defend the Castle");
    window.setFramerateLimit(60);

    MainMenu menu(window);
    MenuAction action = menu.run();

    // Traite l'action retournée ici...
    (void)action;

    return 0;
}