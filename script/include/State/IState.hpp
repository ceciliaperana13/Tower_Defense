#pragma once
#include <SFML/Graphics.hpp>

class IState {
public:
    virtual ~IState() = default;

    virtual void onEnter() {}   // appelé quand l'état devient actif
    virtual void onExit()  {}   // appelé quand on le quitte

    virtual void handleEvents(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    // true = l'état du dessous reste visible (ex: PauseState par-dessus le jeu)
    virtual bool isTransparent() const { return false; }
};