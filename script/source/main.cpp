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

            CountdownTimer    timer(120.f);
            TowerController   towerController;
            towerController.loadFromJson("../assets/data/tower_values.json");

            GameView gameView(window, map, waveManager, timer, towerController);

            // Cercle de surbrillance pour la tour sélectionnée à upgrader
            sf::CircleShape upgradeRing(28.f);
            upgradeRing.setOrigin({ 28.f, 28.f });
            upgradeRing.setFillColor(sf::Color::Transparent);
            upgradeRing.setOutlineColor(sf::Color::Yellow);
            upgradeRing.setOutlineThickness(3.f);
            bool showUpgradeRing = false;
            sf::Vector2f upgradeRingPos;

            clock.restart();

            while (window.isOpen()) {
                float dt = clock.restart().asSeconds();
                if (dt > 0.1f) dt = 0.1f;

                sf::View view = GameView::makeLetterboxView(
                    window.getSize().x, window.getSize().y);
                window.setView(view);

                // ── Événements ───────────────────────────────────────────
                while (const auto event = window.pollEvent()) {

                    if (event->is<sf::Event::Closed>()) {
                        window.close(); break;
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

                            // ── Clic dans l'UI ───────────────────────────
                            std::string type = gameView.getTowerTypeAt(wp);

                            if (!type.empty()) {
                                // Mode upgrade : on applique le choix lv2
                                if (towerController.hasUpgradeTarget()) {
                                    if (type != "basic") {
                                        // Tenter l'upgrade
                                        if (!towerController.upgradeTower(type)) {
                                            // pas assez de coins → feedback console
                                            std::cerr << "[main] Not enough coins to upgrade to "
                                                      << type << "\n";
                                        }
                                        showUpgradeRing = false;
                                    }
                                    // clic sur "basic" en mode upgrade → ignorer
                                }
                                else {
                                    // Mode placement normal
                                    // basic = gratuit, lv2 = 10 coins
                                    if (!towerController.selectTower(type)) {
                                        std::cerr << "[main] Not enough coins for "
                                                  << type << "\n";
                                    }
                                }
                            }
                            else {
                                // ── Clic sur Back / Sell ─────────────────
                                MenuAction act = gameView.handleClickAt(wp);
                                if (act == MenuAction::Exit)
                                    goto backToMenu;

                                // ── Clic sur la carte ─────────────────────
                                if (act == MenuAction::None && wp.y < MAP_H) {

                                    if (towerController.hasSelection()) {
                                        // Poser la tour
                                        towerController.placeTower(wp);
                                        showUpgradeRing = false;
                                    }
                                    else {
                                        // Sélectionner une tour existante pour upgrade
                                        int idx = towerController.getTowerIndexAt(wp);
                                        if (idx >= 0) {
                                            towerController.selectTowerForUpgrade(idx);
                                            // Positionner le ring
                                            // On passe par getTowerIndexAt qui connaît la pos
                                            // On expose getPosition via index dans le controller
                                            showUpgradeRing = true;
                                            upgradeRingPos  = wp; // approx — raffinée ci-dessous
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
                waveManager.update(dt);
                towerController.update(dt, waveManager.getActiveEnemies());
                timer.update(dt);
                gameView.update(dt);

                // ── Rendu ─────────────────────────────────────────────────
                window.clear(sf::Color::Black);
                gameView.render();

                // Ring de surbrillance upgrade
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