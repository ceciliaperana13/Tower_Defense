#include "EndGameManager.hpp"
#include <iostream>

EndGameManager::EndGameManager()
    : state(State::None)
{
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[EndGameManager] Font not found.\n";
}

void EndGameManager::triggerVictory() {
    state = State::Victory;
}

void EndGameManager::triggerDefeat() {
    state = State::Defeat;
}

bool EndGameManager::hasEnded() const {
    return state != State::None;
}

EndGameManager::State EndGameManager::getState() const {
    return state;
}

void EndGameManager::drawPopup(sf::RenderWindow& window) {
    // Fond semi-transparent
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));

    // Titre
    sf::Text title(m_font, "", 64);
    if (state == State::Victory) {
        title.setString("VICTOIRE !");
        title.setFillColor(sf::Color::Green);
    } else {
        title.setString("GAME OVER");
        title.setFillColor(sf::Color::Red);
    }

    auto tb = title.getLocalBounds();
    title.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    title.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f - 50.f });

    // Sous-texte
    sf::Text info(m_font, "Cliquez ou appuyez sur une touche pour continuer", 28);
    auto ib = info.getLocalBounds();
    info.setOrigin({ ib.size.x / 2.f, ib.size.y / 2.f });
    info.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f + 40.f });

    window.draw(overlay);
    window.draw(title);
    window.draw(info);
}

void EndGameManager::showPopup(sf::RenderWindow& window) {
    if (state == State::None)
        return;

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return;
            }
            if (event->is<sf::Event::MouseButtonPressed>() ||
                event->is<sf::Event::KeyPressed>()) {
                return;
            }
        }

        window.clear();
        drawPopup(window);
        window.display();
    }
}
