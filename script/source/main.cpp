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
#include "Model/CountdownTimer.hpp"

static bool s_fullscreen = false;

static sf::View windowedView() {
    return sf::View(sf::FloatRect(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(WIN_W, WIN_H)
    ));
}

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

static void toggleFullscreen(sf::RenderWindow& window) {
    if (s_fullscreen) openWindowed(window);
    else              openFullscreen(window);
}

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
            towerController.loadFromJson("../assets/data/tower_values.json");

            GameView gameView(window, map, waveManager, timer, towerController);

            clock.restart();

            while (window.isOpen()) {
                float dt = clock.restart().asSeconds();
                if (dt > 0.1f) dt = 0.1f;

                while (const auto event = window.pollEvent()) {
                    if (event->is<sf::Event::Closed>())
                        window.close();

                    if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                        if (kp->code == sf::Keyboard::Key::F11)
                            toggleFullscreen(window);

                        if (kp->code == sf::Keyboard::Key::Space &&
                            waveManager.isWaveComplete())
                            waveManager.startNextWave();
                    }

                    sf::View view = GameView::makeLetterboxView(
                        window.getSize().x, window.getSize().y);
                    window.setView(view);

                    if (const auto* mm = event->getIf<sf::Event::MouseMoved>()) {
                        auto worldPos = window.mapPixelToCoords(mm->position, view);
                        gameView.updateHoverAt(worldPos);
                    }
                    else if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                        if (mb->button == sf::Mouse::Button::Left) {
                            auto worldPos = window.mapPixelToCoords(mb->position, view);
                            if (gameView.handleClickAt(worldPos) == MenuAction::Exit)
                                goto backToMenu;
                        }
                    }
                    else {
                        if (gameView.handleEvent(*event) == MenuAction::Exit)
                            goto backToMenu;
                    }
                }

                float dtLogic = dt;
                waveManager.update(dtLogic);
                towerController.update(dtLogic, waveManager.getActiveEnemies());
                timer.update(dtLogic);
                gameView.update(dtLogic);

                sf::View view = GameView::makeLetterboxView(
                    window.getSize().x, window.getSize().y);
                window.setView(view);

                window.clear(sf::Color::Black);
                gameView.render();
                towerController.render(window);
                window.display();
            }

        backToMenu:;
            if (s_fullscreen) openWindowed(window);
        }
    }

    return 0;
}
