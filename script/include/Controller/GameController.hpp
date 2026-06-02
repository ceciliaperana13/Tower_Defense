#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

// #include "Map.hpp"
// #include "GameView.hpp"
// #include "SaveController.hpp"
// #include "SoundManager.hpp"
// #include "Timer.hpp"
// #include "GameState.hpp"

// class GameController {
// public:
//     GameController(sf::RenderWindow& win)
//         : window(win), gameState(GameState::Playing) {}

//     void startGame() {
//         map.load();
//         timer.reset();
//         sound.playMusic("main_theme");
//     }

//     void update(float dt) {
//         if (gameState != GameState::Playing)
//             return;

//         map.update(dt);

//         spawnWave();
//         checkWinLoss();
//     }

//     void onInput(const sf::Event& e) {
//         if (e.is<sf::Event::Closed>())
//             window.close();

//         if (auto* key = e.getIf<sf::Event::KeyPressed>()) {
//             if (key->scancode == sf::Keyboard::Scan::Escape)
//                 window.close();
//         }
//     }

//     void draw() {
//         view.apply(window);
//         map.draw(window);
//     }

// private:
//     void spawnWave() {
//         // logique d'apparition d’ennemis
//     }

//     void checkWinLoss() {
//         // conditions de victoire/défaite
//     }

// private:
//     sf::RenderWindow& window;

//     Map map;
//     GameView view;
//     SaveController save;
//     SoundManager sound;
//     Timer timer;
//     GameState gameState;
// };
