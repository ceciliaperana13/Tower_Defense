#include <SFML/Graphics.hpp>

#include "View/MainMenu.hpp"
#include "View/SettingsMenu.hpp"
#include "View/Leaderboard.hpp"
#include "View/GameView.hpp"

#include "Controller/SaveController.hpp"
#include "Controller/SoundManager.hpp"
#include "Controller/TowerController.hpp"

#include "Model/Map.hpp"
#include "Model/WaveManager.hpp"
#include "Model/Enemy.hpp"

static bool s_fullscreen = false;

// ─────────────────────────────────────────────
// Vue fenêtrée
// ─────────────────────────────────────────────
static sf::View windowedView() {
    return sf::View(sf::FloatRect(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(WIN_W, WIN_H)
    ));
}

// ─────────────────────────────────────────────
// Ouvrir en mode fenêtré
// ─────────────────────────────────────────────
static void openWindowed(sf::RenderWindow& window) {
    window.create(
        sf::VideoMode(sf::Vector2u(WIN_W, WIN_H)),
        "Defend the Castle",
        sf::State::Windowed
    );
    window.setFramerateLimit(60);
    window.setView(windowedView());
    s_fullscreen = false;
}

// ─────────────────────────────────────────────
// Ouvrir en plein écran
// ─────────────────────────────────────────────
static void openFullscreen(sf::RenderWindow& window) {
    auto desk = sf::VideoMode::getDesktopMode();

    window.create(
        sf::VideoMode(sf::Vector2u(desk.size.x, desk.size.y)),
        "Defend the Castle",
        sf::State::Fullscreen
    );
    window.setFramerateLimit(60);

    window.setView(sf::View(sf::FloatRect(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(WIN_W, WIN_H)
    )));

    s_fullscreen = true;
}

// ─────────────────────────────────────────────
// Toggle fullscreen
// ─────────────────────────────────────────────
static void toggleFullscreen(sf::RenderWindow& window) {
    if (s_fullscreen) openWindowed(window);
    else              openFullscreen(window);
}

// ─────────────────────────────────────────────
// Rafraîchir la vue
// ─────────────────────────────────────────────
static void refreshView(sf::RenderWindow& window) {
    if (s_fullscreen) {
        window.setView(sf::View(sf::FloatRect(
            sf::Vector2f(0.f, 0.f),
            sf::Vector2f(WIN_W, WIN_H)
        )));
    } else {
        window.setView(windowedView());
    }
}

// ─────────────────────────────────────────────
// MAIN
// ─────────────────────────────────────────────
int main() {
    sf::RenderWindow window;
    openWindowed(window);

    GameSettings   settings;
    SaveController saveCtrl;
    sf::Clock      clock;

    while (window.isOpen()) {
        refreshView(window);

        MainMenu menu(window);
        MenuAction action = menu.run();

        if (action == MenuAction::Exit || !window.isOpen())
            break;

        if (action == MenuAction::Settings) {
            refreshView(window);
            SettingsMenu sm(window, settings);
            sm.run();
            continue;
        }

        if (action == MenuAction::Scoreboard) {
            refreshView(window);
            Leaderboard lb(window, saveCtrl);
            lb.run();
            continue;
        }

        if (action == MenuAction::NewGame) {

            Map map;
            Enemy::loadHeartTexture("../assets/sprites/icons/heart.png");

            auto waypoints = map.getWaypoints(MAP_SCALE);

            WaveManager waveManager(
                "../assets/data/waves.json",
                "../assets/data/enemy_values.json",
                waypoints
            );
            waveManager.startNextWave();

            CountdownTimer timer(120.f);

            TowerController towerController;
            towerController.loadFromJson("../assets/data/towers.json");

            GameView gameView(window, map, waveManager, timer, towerController);

            clock.restart();

            while (window.isOpen()) {
                float dt = clock.restart().asSeconds();
                if (dt > 0.1f) dt = 0.1f;

                while (const auto event = window.pollEvent()) {

                    // Fermeture fenêtre
                    if (event->is<sf::Event::Closed>())
                        window.close();

                    // Touche clavier
                    if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                        if (kp->code == sf::Keyboard::Key::F11)
                            toggleFullscreen(window);

                        if (kp->code == sf::Keyboard::Key::Space &&
                            waveManager.isWaveComplete())
                            waveManager.startNextWave();
                    }

                    sf::View view = window.getView();

                    // Souris déplacée
                    if (const auto* mm = event->getIf<sf::Event::MouseMoved>()) {
                        gameView.updateHoverAt(
                            window.mapPixelToCoords(mm->position, view)
                        );
                    }

                    // Clic souris
                    else if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                        if (mb->button == sf::Mouse::Button::Left) {
                            auto pos = window.mapPixelToCoords(mb->position, view);
                            if (gameView.handleClickAt(pos) == MenuAction::Exit)
                                goto backToMenu;
                        }
                    }

                    // Autres events
                    else {
                        if (gameView.handleEvent(*event) == MenuAction::Exit)
                            goto backToMenu;
                    }
                }

                gameView.update(dt);

                refreshView(window);
                window.clear(sf::Color::Black);
                gameView.render();
                window.display();
            }

        backToMenu:;
            if (s_fullscreen) openWindowed(window);
        }
    }

    return 0;
}
