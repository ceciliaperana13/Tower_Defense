#include "MainMenu.hpp"
#include <iostream>
#include <stdexcept>


// ─── MenuButton helpers 
bool MenuButton::contains(sf::Vector2f point) const {
    return shape.getGlobalBounds().contains(point);
}

void MenuButton::setHovered(bool hovered) {
    shape.setFillColor(hovered ? sf::Color(180, 180, 180, 255) : sf::Color(255, 255, 255, 255));
}

// ─── Constructeur 
MainMenu::MainMenu(sf::RenderWindow& window)
    : m_window(window)
{
    loadAssets();
    buildButtons();
}

// ─── loadAssets 
void MainMenu::loadAssets() {
    // Police
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        throw std::runtime_error("MainMenu: impossible de charger la police.");

    // Background
    if (!m_bgTexture.loadFromFile("../assets/sprites/background.jpg")) {
        std::cerr << "[MainMenu] background.jpg introuvable.\n";
    } else {
        m_bgSprite.emplace(m_bgTexture);
        float sx = static_cast<float>(m_window.getSize().x) /
                   static_cast<float>(m_bgTexture.getSize().x);
        float sy = static_cast<float>(m_window.getSize().y) /
                   static_cast<float>(m_bgTexture.getSize().y);
        m_bgSprite->setScale({ sx, sy });
    }

    // Titre
    m_title.emplace(m_font, "Defend the Castle", 72u);
    m_title->setFillColor(sf::Color::White);
    m_title->setStyle(sf::Text::Bold);
    sf::FloatRect tb = m_title->getLocalBounds();
    float titleX = (static_cast<float>(m_window.getSize().x) - tb.size.x) / 2.f - tb.position.x;
    float titleY = static_cast<float>(m_window.getSize().y) * 0.10f;
    m_title->setPosition({ titleX, titleY });

    // Icône settings
    if (!m_settingsTexture.loadFromFile("../assets/sprites/setting.png")) {
        std::cerr << "[MainMenu] setting.png introuvable.\n";
    } else {
        m_settingsSprite.emplace(m_settingsTexture);
        float scale = 48.f / static_cast<float>(m_settingsTexture.getSize().x);
        m_settingsSprite->setScale({ scale, scale });
        m_settingsSprite->setPosition({ 12.f, 12.f });
    }
}

// buildButtons 
void MainMenu::buildButtons() {
    const float winW = static_cast<float>(m_window.getSize().x);
    const float winH = static_cast<float>(m_window.getSize().y);
    const float btnW = 300.f;
    const float btnH = 80.f;
    const float cx   = winW / 2.f;

    // Réserve la mémoire d'abord pour éviter les réallocations
    m_buttons.reserve(4);

    m_buttons.push_back(makeButton("../assets/sprites/buttons/new_game.png",
                                   cx, winH * 0.38f, btnW, btnH, MenuAction::NewGame));
    m_buttons.push_back(makeButton("../assets/sprites/buttons/leaderboard.png",
                                   cx, winH * 0.52f, btnW, btnH, MenuAction::Scoreboard));
    m_buttons.push_back(makeButton("../assets/sprites/buttons/options.png",
                                   cx, winH * 0.66f, btnW, btnH, MenuAction::Settings));
    m_buttons.push_back(makeButton("../assets/sprites/buttons/quit_game.png",
                                   cx, winH * 0.80f, btnW, btnH, MenuAction::Exit));

    
    for (auto& btn : m_buttons)
        btn.shape.setTexture(&btn.texture);
}

//  makeButton
MenuButton MainMenu::makeButton(const std::string& texturePath,
                                float cx, float cy,
                                float width, float height,
                                MenuAction action)
{
    MenuButton btn;
    btn.action = action;

    if (!btn.texture.loadFromFile(texturePath))
        std::cerr << "[MainMenu] Texture introuvable : " << texturePath << "\n";

    btn.shape.setSize({ width, height });
    btn.shape.setOrigin({ width / 2.f, height / 2.f });
    btn.shape.setPosition({ cx, cy });
    btn.shape.setFillColor(sf::Color::White);
    

    return btn;
}

// ─── run 
MenuAction MainMenu::run() {
    MenuAction result = MenuAction::None;
    m_running = true;

    while (m_running && m_window.isOpen()) {
        sf::Vector2f mouse = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
        handleEvents(result);
        update(mouse);
        render();
    }

    return result;
}

// ─── handleEvents 
void MainMenu::handleEvents(MenuAction& result) {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {

        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            result    = MenuAction::Exit;
            m_running = false;
            return;
        }

        if (const auto* mb = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mb->button == sf::Mouse::Button::Left) {
                sf::Vector2f pos = m_window.mapPixelToCoords({ mb->position.x, mb->position.y });

                for (auto& btn : m_buttons) {
                    if (btn.contains(pos)) {
                        result    = btn.action;
                        m_running = false;
                        return;
                    }
                }

                if (m_settingsSprite.has_value() &&
                    m_settingsSprite->getGlobalBounds().contains(pos))
                {
                    result    = MenuAction::Settings;
                    m_running = false;
                    return;
                }
            }
        }

        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Escape) {
                result    = MenuAction::Exit;
                m_running = false;
            }
        }
    }
}

// ─── update 
void MainMenu::update(sf::Vector2f mousePos) {
    for (auto& btn : m_buttons) {
        btn.setHovered(btn.contains(mousePos));
    }
}

// ─── render 
void MainMenu::render() {
    m_window.clear(sf::Color(20, 20, 40));

    if (m_bgSprite.has_value())
        m_window.draw(*m_bgSprite);

    if (m_title.has_value()) {
        sf::Text shadow = *m_title;
        shadow.setFillColor(sf::Color(0, 0, 0, 150));
        shadow.move({ 3.f, 3.f });
        m_window.draw(shadow);
        m_window.draw(*m_title);
    }

    for (auto& btn : m_buttons)
        m_window.draw(btn.shape);

    if (m_settingsSprite.has_value())
        m_window.draw(*m_settingsSprite);

    m_window.display();
}