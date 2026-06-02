#include "View/MainMenu.hpp"
#include "View/SettingsMenu.hpp"
#include "View/Leaderboard.hpp"
#include "View/GameView.hpp"
#include "Controller/SaveController.hpp"
#include "Model/Map.hpp"

static bool s_fullscreen = false;

static sf::View buildView(unsigned sw, unsigned sh) {
    if (sw == GameView::WIN_W && sh == GameView::WIN_H)
        return sf::View(sf::FloatRect({ 0.f, 0.f },
                        { static_cast<float>(GameView::WIN_W),
                          static_cast<float>(GameView::WIN_H) }));
    return GameView::makeLetterboxView(sw, sh);
}

// Rebuilds the window and immediately sets the correct view
static void openWindowed(sf::RenderWindow& window) {
    window.create(sf::VideoMode({ GameView::WIN_W, GameView::WIN_H }),
                  "Defend the Castle");
    window.setFramerateLimit(60);
    window.setView(buildView(GameView::WIN_W, GameView::WIN_H));
    s_fullscreen = false;
}

static void openFullscreen(sf::RenderWindow& window) {
    auto desk = sf::VideoMode::getDesktopMode();
    window.create(desk, "Defend the Castle", sf::State::Fullscreen);
    window.setFramerateLimit(60);
    window.setView(buildView(desk.size.x, desk.size.y));
    s_fullscreen = true;
}

static void toggleFullscreen(sf::RenderWindow& window) {
    if (s_fullscreen) openWindowed(window);
    else              openFullscreen(window);
}

// Must be called each frame to re-assert the view (window.create resets it)
static void refreshView(sf::RenderWindow& window) {
    auto sz = window.getSize();
    window.setView(buildView(sz.x, sz.y));
}

int main() {
    sf::RenderWindow window;
    openWindowed(window);

    GameSettings   settings;
    SaveController saveCtrl;

    while (window.isOpen()) {
        // View must be set before MainMenu reads window.getView().getSize()
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
            Map      map;
            GameView gameView(window, map);

            while (window.isOpen()) {
                while (const auto event = window.pollEvent()) {
                    if (event->is<sf::Event::Closed>())
                        window.close();

                    if (const auto* kp = event->getIf<sf::Event::KeyPressed>())
                        if (kp->code == sf::Keyboard::Key::F11)
                            toggleFullscreen(window);

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