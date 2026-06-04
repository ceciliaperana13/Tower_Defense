#include <SFML/Graphics.hpp>
#include <iostream>

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
#include "Model/Castle.hpp"

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

int main() {
    sf::RenderWindow window;
    openWindowed(window);

    GameSettings   settings;
    SaveController saveCtrl;
    sf::Clock      clock;

    while (window.isOpen()) {
        window.setView(windowedView());

        MainMenu menu(window);
        MenuAction action = menu.run();

        if (action == MenuAction::Exit || !window.isOpen()) break;

        if (action == MenuAction::Settings) {
            window.setView(windowedView());
            SettingsMenu sm(window, settings);
            sm.run();
            continue;
        }
        if (action == MenuAction::Scoreboard) {
            window.setView(windowedView());
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

            CountdownTimer  timer(120.f);
            TowerController towerController;
            towerController.loadFromJson("../assets/data/tower_values.json");

            // ── Château positionné sur le dernier waypoint ────────────────
            sf::Vector2f castlePos = waypoints.empty()
                ? sf::Vector2f(900.f, 550.f)
                : waypoints.back();

            Castle castle(
                "../assets/sprites/big_buildings/castle.png",
                castlePos,
                20
            );

            GameView gameView(window, map, waveManager, timer, towerController);
            gameView.setLives(castle.getLives());

            // ── Cercle surbrillance upgrade ───────────────────────────────
            sf::CircleShape upgradeRing(28.f);
            upgradeRing.setOrigin({ 28.f, 28.f });
            upgradeRing.setFillColor(sf::Color::Transparent);
            upgradeRing.setOutlineColor(sf::Color::Yellow);
            upgradeRing.setOutlineThickness(3.f);
            bool         showUpgradeRing = false;
            sf::Vector2f upgradeRingPos;

            int prevReached = 0;
            clock.restart();

            while (window.isOpen()) {
                float dt = clock.restart().asSeconds();
                if (dt > 0.1f) dt = 0.1f;

                sf::View view = GameView::makeLetterboxView(
                    window.getSize().x, window.getSize().y);
                window.setView(view);

                // ── Événements ────────────────────────────────────────────
                while (const auto event = window.pollEvent()) {

                    if (event->is<sf::Event::Closed>()) {
                        window.close();
                        break;
                    }

                    if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                        if (kp->code == sf::Keyboard::Key::F11)
                            toggleFullscreen(window);

                        if (kp->code == sf::Keyboard::Key::Escape) {
                            towerController.clearSelection();
                            showUpgradeRing = false;
                        }

                        if (kp->code == sf::Keyboard::Key::Space &&
                            waveManager.isWaveComplete())
                            waveManager.startNextWave();
                    }

                    if (const auto* mm = event->getIf<sf::Event::MouseMoved>()) {
                        auto wp = window.mapPixelToCoords(mm->position, view);
                        towerController.setGhostPosition(wp);
                        gameView.updateHoverAt(wp);
                    }

                    if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                        auto wp = window.mapPixelToCoords(mb->position, view);

                        if (mb->button == sf::Mouse::Button::Left) {

                            std::string type = gameView.getTowerTypeAt(wp);

                            if (!type.empty()) {
                                if (towerController.hasUpgradeTarget()) {
                                    if (type != "basic") {
                                        if (!towerController.upgradeTower(type))
                                            std::cerr << "[main] Not enough coins to upgrade to "
                                                      << type << "\n";
                                        showUpgradeRing = false;
                                    }
                                } else {
                                    if (!towerController.selectTower(type))
                                        std::cerr << "[main] Not enough coins for "
                                                  << type << "\n";
                                }
                            } else {
                                MenuAction act = gameView.handleClickAt(wp);
                                if (act == MenuAction::Exit)
                                    goto backToMenu;

                                if (act == MenuAction::None && wp.y < MAP_H) {
                                    if (towerController.hasSelection()) {
                                        towerController.placeTower(wp);
                                        showUpgradeRing = false;
                                    } else {
                                        int idx = towerController.getTowerIndexAt(wp);
                                        if (idx >= 0) {
                                            towerController.selectTowerForUpgrade(idx);
                                            showUpgradeRing = true;
                                            upgradeRingPos  = wp;
                                        } else {
                                            towerController.clearSelection();
                                            showUpgradeRing = false;
                                        }
                                    }
                                }
                            }
                        }

                        if (mb->button == sf::Mouse::Button::Right) {
                            towerController.clearSelection();
                            showUpgradeRing = false;
                        }
                    }
                }

                // ── Logique ───────────────────────────────────────────────
                // ── Gain de coins sur kill (avant update qui supprime les ennemis) ──
                for (auto& e : waveManager.getActiveEnemies()) {
                    if (e->isDead() && !e->hasReached())
                        towerController.addCoins(e->getReward());
                }

                waveManager.update(dt);

                // Vies château : décrémenter selon ennemis arrivés
                {
                    int totalReached = waveManager.getTotalReached();
                    int delta        = totalReached - prevReached;
                    if (delta > 0) {
                        castle.loseLife(delta);
                        gameView.setLives(castle.getLives());
                        prevReached = totalReached;
                        std::cout << "[main] Castle lives: " << castle.getLives() << "\n";
                    }
                }

                if (castle.isDead()) {
                    std::cout << "[main] Game Over!\n";
                    goto backToMenu;
                }

                towerController.update(dt, waveManager.getActiveEnemies());
                timer.update(dt);
                gameView.update(dt);

                // ── Rendu ─────────────────────────────────────────────────
                window.clear(sf::Color::Black);
                gameView.render();          // carte + ennemis + UI bar

                castle.render(window);      // sprite château + barre de vie

                if (showUpgradeRing && towerController.hasUpgradeTarget()) {
                    upgradeRing.setPosition(upgradeRingPos);
                    window.draw(upgradeRing);
                }

                towerController.render(window);
                window.display();
            }

        backToMenu:;
            if (s_fullscreen) openWindowed(window);
        }
    }

    return 0;
}