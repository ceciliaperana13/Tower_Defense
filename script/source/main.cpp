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
        sf::Vector2f(float(WIN_W), float(WIN_H))
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
        sf::Vector2f(float(WIN_W), float(WIN_H))
    )));
    s_fullscreen = true;
}

static void toggleFullscreen(sf::RenderWindow& window) {
    if (s_fullscreen) openWindowed(window);
    else              openFullscreen(window);
}

static void refreshView(sf::RenderWindow& window) {
    window.setView(sf::View(sf::FloatRect(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(float(WIN_W), float(WIN_H))
    )));
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

                // Vue letterbox recalculée à chaque frame
                sf::View view = GameView::makeLetterboxView(
                    window.getSize().x, window.getSize().y);
                window.setView(view);

                // ── Événements ───────────────────────────────────────────
                while (const auto event = window.pollEvent()) {

                    if (event->is<sf::Event::Closed>()) {
                        window.close();
                        break;
                    }

                    if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                        if (kp->code == sf::Keyboard::Key::F11)
                            toggleFullscreen(window);

                        // Annule la sélection de tour en cours
                        if (kp->code == sf::Keyboard::Key::Escape)
                            towerController.selectTower("");

                        // Vague suivante
                        if (kp->code == sf::Keyboard::Key::Space &&
                            waveManager.isWaveComplete())
                            waveManager.startNextWave();
                    }

                    if (const auto* mm = event->getIf<sf::Event::MouseMoved>()) {
                        auto worldPos = window.mapPixelToCoords(mm->position, view);
                        towerController.setGhostPosition(worldPos);
                        gameView.updateHoverAt(worldPos);
                    }

                    if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                        auto worldPos = window.mapPixelToCoords(mb->position, view);

                        if (mb->button == sf::Mouse::Button::Left) {

                            // 1) Clic sur un bouton de tour dans l'UI ?
                            std::string type = gameView.getTowerTypeAt(worldPos);
                            if (!type.empty()) {
                                towerController.selectTower(type);
                            }
                            else {
                                // 2) Clic sur Back / Sell ?
                                MenuAction act = gameView.handleClickAt(worldPos);
                                if (act == MenuAction::Exit)
                                    goto backToMenu;

                                // 3) Clic sur la carte → poser la tour
                                if (act == MenuAction::None &&
                                    towerController.hasSelection() &&
                                    worldPos.y < MAP_H)
                                {
                                    towerController.placeTower(worldPos);
                                }
                            }
                        }

                        // Clic droit = annuler sélection
                        if (mb->button == sf::Mouse::Button::Right)
                            towerController.selectTower("");
                    }
                }

                // ── Logique ───────────────────────────────────────────────
                waveManager.update(dt);
                towerController.update(dt, waveManager.getActiveEnemies());
                timer.update(dt);
                gameView.update(dt);

                // ── Rendu ─────────────────────────────────────────────────
                window.clear(sf::Color::Black);
                gameView.render();              // map + ennemis + UI
                towerController.render(window); // tours + projectiles + fantôme
                window.display();
            }

        backToMenu:;
            if (s_fullscreen) openWindowed(window);
        }
    }

    return 0;
}