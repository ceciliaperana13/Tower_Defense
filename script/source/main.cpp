#include "View/MainMenu.hpp"
#include "View/SettingsMenu.hpp"
#include "View/Leaderboard.hpp"
#include "View/GameView.hpp"
#include "Controller/SaveController.hpp"
#include "Model/Map.hpp"
#include "Model/WaveManager.hpp"
#include "Model/Enemy.hpp"

static bool s_fullscreen = false;

static sf::View windowedView() {
    return sf::View(sf::FloatRect({ 0.f, 0.f },
                    { static_cast<float>(GameView::WIN_W),
                      static_cast<float>(GameView::WIN_H) }));
}

static void openWindowed(sf::RenderWindow& window) {
    window.create(sf::VideoMode({ GameView::WIN_W, GameView::WIN_H }),
                  "Defend the Castle");
    window.setFramerateLimit(60);
    window.setView(windowedView());
    s_fullscreen = false;
}

static void openFullscreen(sf::RenderWindow& window) {
    auto desk = sf::VideoMode::getDesktopMode();
    window.create(desk, "Defend the Castle", sf::State::Fullscreen);
    window.setFramerateLimit(60);
    window.setView(GameView::makeLetterboxView(desk.size.x, desk.size.y));
    s_fullscreen = true;
}

static void toggleFullscreen(sf::RenderWindow& window) {
    if (s_fullscreen) openWindowed(window);
    else              openFullscreen(window);
}

static void refreshView(sf::RenderWindow& window) {
    if (s_fullscreen) {
        auto sz = window.getSize();
        window.setView(GameView::makeLetterboxView(sz.x, sz.y));
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

        MainMenu   menu(window);
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

            // Charge la texture cœur une seule fois pour tous les ennemis
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
                        if (kp->code == sf::Keyboard::Key::Space
                            && waveManager.isWaveComplete())
                            waveManager.startNextWave();
                    }

                    sf::View view = window.getView();
                    if (const auto* mm = event->getIf<sf::Event::MouseMoved>()) {
                        gameView.updateHoverAt(
                            window.mapPixelToCoords(mm->position, view));
                    } else if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                        if (mb->button == sf::Mouse::Button::Left) {
                            auto pos = window.mapPixelToCoords(mb->position, view);
                            if (gameView.handleClickAt(pos) == MenuAction::Exit)
                                goto backToMenu;
                        }
                    } else {
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