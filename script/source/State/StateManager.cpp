//va servir spécifiquement pour la Pause 
#include "State/StateManager.hpp"

void StateManager::change(std::unique_ptr<IState> state) {
    if (!m_stack.empty()) {
        m_stack.back()->onExit();
        m_stack.pop_back();
    }
    state->onEnter();
    m_stack.push_back(std::move(state));
}

void StateManager::push(std::unique_ptr<IState> state) {
    if (!m_stack.empty())
        m_stack.back()->onExit();
    state->onEnter();
    m_stack.push_back(std::move(state));
}

void StateManager::pop() {
    if (m_stack.empty()) return;
    m_stack.back()->onExit();
    m_stack.pop_back();
    if (!m_stack.empty())
        m_stack.back()->onEnter();
}

void StateManager::handleEvents(const sf::Event& event) {
    if (!m_stack.empty())
        m_stack.back()->handleEvents(event);
}

void StateManager::update(float dt) {
    if (!m_stack.empty())
        m_stack.back()->update(dt);
}

void StateManager::render(sf::RenderWindow& window) {
    // Si l'état du dessus est transparent (Pause),
    // on render aussi l'état en dessous
    for (std::size_t i = 0; i < m_stack.size(); ++i) {
        bool isTop = (i == m_stack.size() - 1);
        if (isTop || m_stack.back()->isTransparent())
            m_stack[i]->render(window);
    }
}