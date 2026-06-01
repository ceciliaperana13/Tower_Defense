#include "MainMenu.hpp"
#include <iostream>
#include <stdexcept>


//  MenuButton helpers
bool MenuButton::contains(sf::Vector2f point) const {
    return shape.getGlobalBounds().contains(point);
}

void MenuButton::setHovered(bool hovered) {
    shape.setFillColor(hovered ? hoverColor : normalColor);
}

//  Constructeur
MainMenu::MainMenu(sf::RenderWindow& window)
    : m_window(window)
{
    loadAssets();    // charge m_font en premier
    buildButtons();
    buildSettingsIcon();
}

// ═══════════════════════════════════════════════════════════════════
//  Chargement des assets
// ═══════════════════════════════════════════════════════════════════

void MainMenu::loadAssets() {
    // ── Police ─────────────────────────────────────────────────────
    if (!m_font.openFromFile("assets/fonts/bold.ttf")) {
        if (!m_font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf") &&
            !m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        {
            throw std::runtime_error("MainMenu: impossible de charger la police.");
        }
    }

    // ── Background ─────────────────────────────────────────────────
    if (!m_bgTexture.loadFromFile("assets/sprites/background.jpg")) {
        std::cerr << "[MainMenu] Warning: background introuvable, fond uni.\n";
    } else {
        m_bgSprite.emplace(m_bgTexture);
        float sx = static_cast<float>(m_window.getSize().x) /
                   static_cast<float>(m_bgTexture.getSize().x);
        float sy = static_cast<float>(m_window.getSize().y) /
                   static_cast<float>(m_bgTexture.getSize().y);
        m_bgSprite->setScale({ sx, sy });
    }

    // ── Titre — construit avec la font (SFML 3) ────────────────────
    m_title.emplace(m_font, "Defend the Castle", 72u);
    m_title->setFillColor(sf::Color::Black);
    m_title->setStyle(sf::Text::Bold);

    sf::FloatRect tb = m_title->getLocalBounds();
    float titleX = (static_cast<float>(m_window.getSize().x) - tb.size.x) / 2.f - tb.position.x;
    float titleY = static_cast<float>(m_window.getSize().y) * 0.22f;
    m_title->setPosition({ titleX, titleY });

    // ── Icône settings ─────────────────────────────────────────────
    if (m_settingsTexture.loadFromFile("assets/sprites/settings_icon.png")) {
        m_settingsSprite.emplace(m_settingsTexture);
        m_settingsSprite->setPosition({ 12.f, 12.f });
        float scale = 48.f / static_cast<float>(m_settingsTexture.getSize().x);
        m_settingsSprite->setScale({ scale, scale });
        m_hasSettingsTexture = true;
    }
}


//  Construction des bouton
void MainMenu::buildButtons() {
    const float winW = static_cast<float>(m_window.getSize().x);
    const float winH = static_cast<float>(m_window.getSize().y);
    const float btnW = 220.f;
    const float btnH = 55.f;
    const float cx   = winW / 2.f;

    sf::Color normalDark (30,  30,  30,  210);
    sf::Color hoverDark  (60,  60,  60,  230);
    sf::Color normalRed  (200, 30,  30,  230);
    sf::Color hoverRed   (230, 60,  60,  255);

    m_buttons.push_back(makeButton("New Game",   cx, winH * 0.62f, btnW, btnH,
                                   MenuAction::NewGame,    normalDark, hoverDark));
    m_buttons.push_back(makeButton("Scoreboard", cx, winH * 0.73f, btnW, btnH,
                                   MenuAction::Scoreboard, normalDark, hoverDark));
    m_buttons.push_back(makeButton("EXIT",       cx, winH * 0.84f, btnW, btnH,
                                   MenuAction::Exit,       normalRed,  hoverRed));
}


//  Icône Settings
void MainMenu::buildSettingsIcon() {
    m_settingsIcon.setSize({ 48.f, 48.f });
    m_settingsIcon.setPosition({ 12.f, 12.f });
    m_settingsIcon.setFillColor(sf::Color(50, 50, 50, 180));
    m_settingsIcon.setOutlineColor(sf::Color(200, 200, 200, 150));
    m_settingsIcon.setOutlineThickness(1.5f);
}


//  Fabrique un bouton centré
MenuButton MainMenu::makeButton(const std::string& text,
                                float cx, float cy,
                                float width, float height,
                                MenuAction action,
                                sf::Color normal, sf::Color hover)
{
    MenuButton btn;
    btn.normalColor = normal;
    btn.hoverColor  = hover;
    btn.action      = action;

    btn.shape.setSize({ width, height });
    btn.shape.setOrigin({ width / 2.f, height / 2.f });
    btn.shape.setPosition({ cx, cy });
    btn.shape.setFillColor(normal);

    // SFML 3 : sf::Text construit avec font + string + taille
    btn.label.emplace(m_font, text, 24u);
    btn.label->setFillColor(sf::Color::White);
    btn.label->setStyle(sf::Text::Regular);

    sf::FloatRect lb = btn.label->getLocalBounds();
    btn.label->setOrigin({ lb.position.x + lb.size.x / 2.f,
                           lb.position.y + lb.size.y / 2.f });
    btn.label->setPosition({ cx, cy });

    return btn;
}


//  Boucle principale
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


//  Gestion des événements
void MainMenu::handleEvents(MenuAction& result) {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {

        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            result    = MenuAction::Exit;
            m_running = false;
            return;
        }

        if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseReleased->button == sf::Mouse::Button::Left) {
                sf::Vector2f pos = m_window.mapPixelToCoords(
                    { mouseReleased->position.x, mouseReleased->position.y });

                for (auto& btn : m_buttons) {
                    if (btn.contains(pos)) {
                        result    = btn.action;
                        m_running = false;
                        return;
                    }
                }

                if (m_settingsIcon.getGlobalBounds().contains(pos)) {
                    result    = MenuAction::Settings;
                    m_running = false;
                    return;
                }
            }
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                result    = MenuAction::Exit;
                m_running = false;
            }
        }
    }
}

//  Update
void MainMenu::update(sf::Vector2f mousePos) {
    for (auto& btn : m_buttons) {
        btn.setHovered(btn.contains(mousePos));
    }

    bool settingsHover = m_settingsIcon.getGlobalBounds().contains(mousePos);
    m_settingsIcon.setFillColor(settingsHover
        ? sf::Color(90, 90, 90, 200)
        : sf::Color(50, 50, 50, 180));
}


//  Rendu
void MainMenu::render() {
    m_window.clear(sf::Color(100, 149, 237));

    if (m_bgSprite.has_value())
        m_window.draw(*m_bgSprite);

    if (m_title.has_value())
        m_window.draw(*m_title);

    for (auto& btn : m_buttons) {
        m_window.draw(btn.shape);
        if (btn.label.has_value())
            m_window.draw(*btn.label);
    }

    if (m_hasSettingsTexture && m_settingsSprite.has_value())
        m_window.draw(*m_settingsSprite);
    else
        m_window.draw(m_settingsIcon);

    m_window.display();
}