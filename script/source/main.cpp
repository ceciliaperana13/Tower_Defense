#include <SFML/Graphics.hpp>

#include "View/MainMenu.hpp"
#include "View/SettingsMenu.hpp"
#include "View/Leaderboard.hpp"
#include "View/GameView.hpp"

#include "Controller/SaveController.hpp"
#include "Controller/SoundManager.hpp"

#include "Model/Map.hpp"
#include "Model/WaveManager.hpp"
#include "Model/Enemy.hpp"

// ─────────────────────────────────────────────
// CONSTANTES D'ÉCRAN
// ─────────────────────────────────────────────
constexpr unsigned WIN_W = 960;
constexpr unsigned WIN_H = 540;

static bool s_fullscreen = false;

// ─────────────────────────────────────────────
// Vue fenêtrée
// ─────────────────────────────────────────────
static sf::View windowedView() {
    return sf::View(
        sf::FloatRect(
            sf::Vector2f(0.f, 0.f),
            sf::Vector2f(static_cast<float>(WIN_W),
                         static_cast<float>(WIN_H))
        )
    );
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

    window.setView(sf::View(
        sf::FloatRect(
            sf::Vector2f(0.f, 0.f),
            sf::Vector2f(static_cast<float>(WIN_W),
                         static_cast<float>(WIN_H))
        )
    ));

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
        auto sz = window.getSize();
        window.setView(sf::View(
            sf::FloatRect(
                sf::Vector2f(0.f, 0.f),
                sf::Vector2f(static_cast<float>(WIN_W),
                             static_cast<float>(WIN_H))
            )
        ));
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

            constexpr float MAP_SCALE = 2.f;
            auto waypoints = map.getWaypoints(MAP_SCALE);

            WaveManager waveManager(
                "../assets/data/waves.json",
                "../assets/data/enemy_values.json",
                waypoints
            );
            waveManager.startNextWave();

            CountdownTimer timer(120.f);
            GameView gameView(window, map, waveManager, timer);
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

                    sf::View view = window.getView();

                    if (const auto* mm = event->getIf<sf::Event::MouseMoved>()) {
                        gameView.updateHoverAt(
                            window.mapPixelToCoords(mm->position, view));
                    }
                    else if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                        if (mb->button == sf::Mouse::Button::Left) {
                            auto pos = window.mapPixelToCoords(mb->position, view);
                            if (gameView.handleClickAt(pos) == MenuAction::Exit)
                                goto backToMenu;
                        }
                    }
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
