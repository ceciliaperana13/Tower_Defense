        // // GameController complet avec Map, GameView, SoundManager, Timer, GameState a gerer en fxml3
        // #include "GameController.hpp"
        // #include "SoundManager.hpp"
        // #include <iostream>


        // //---Constructeur
        // GameController::GameController(sf::RenderWindow& window)
        //     : m_window(window)
        // {
        //     // TODO : initialiser la map, la vue, le timer, le SoundManager, etc.
        // }
        // //---run
        // void GameController::run(){
        //     SoundManager::getInstance().playMusic("main_theme");

        //     sf::Clock clock;
        //     while (m_running && m_window.isOpen()) {
        //         sf::Time dt = clock.restart();

        //         // TODO : gérer les événements, mettre à jour la logique du jeu, et rendre la vue
        //         // Exemple de gestion d'événement basique :
        //         sf::Event event;
        //         while (m_window.pollEvent(event)) {
        //             if (event.type == sf::Event::Closed) {
        //                 m_window.close();
        //             }
        //             // TODO : gérer les autres événements (input, etc.)
        //         }

        //         // TODO : mettre à jour la logique du jeu avec dt.asSeconds()

        //         m_window.clear();
        //         // TODO : dessiner la vue du jeu
        //         m_window.display();
        //     }
        //     // TODO : faire les nettoyages nécessaires (sauvegarder la partie, etc.)
        //     SoundManager::getInstance().stopMusic();
        //     // Todo : éventuellement jouer un son de fin de partie, afficher un écran de Game Over, etc.

        // }