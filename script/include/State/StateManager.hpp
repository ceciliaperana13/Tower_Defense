#pragma once
#include <vector>
#include <memory>
#include "State/IState.hpp"

class StateManager {
public:
    // Remplace l'état actuel (ex: Menu → Game)
    void change(std::unique_ptr<IState> state);

    // Empile un état par-dessus (ex: Game → Pause)
    void push(std::unique_ptr<IState> state);

    // Dépile l'état actuel (ex: Pause → retour Game)
    void pop();

    void handleEvents(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderWindow& window);

    bool isEmpty() const { return m_stack.empty(); }

private:
    std::vector<std::unique_ptr<IState>> m_stack;
};