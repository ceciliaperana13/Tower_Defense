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
    loadAssets();
    buildButtons();
    buildSettingsIcon();
}

// ═══════════════════════════════════════════════════════════════════
//  Chargement des assets
// ═══════════════════════════════════════════════════════════════════

void MainMenu::loadAssets() {
    // ── Police ─────────────────────────────────────────────────────
    // Priorité : assets/fonts/bold.ttf  →  police système de secours
    if (!m_font.loadFromFile("assets/fonts/bold.ttf")) {
        // Tentative police système Linux / Windows
        if (!m_font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf") &&
            !m_font.loadFromFile("C:/Windows/Fonts/arialbd.ttf"))
        {
            throw std::runtime_error("MainMenu: impossible de charger la police.");
        }
    }

    // ── Background ─────────────────────────────────────────────────
    // Fichier attendu : assets/sprites/background.png
    if (!m_bgTexture.loadFromFile("assets/sprites/background.png")) {
        std::cerr << "[MainMenu] Warning: background introuvable, fond uni.\n";
    } else {
        m_bgSprite.setTexture(m_bgTexture);

        // Redimensionne le sprite pour couvrir toute la fenêtre
        float sx = static_cast<float>(m_window.getSize().x) /
                   static_cast<float>(m_bgTexture.getSize().x);
        float sy = static_cast<float>(m_window.getSize().y) /
                   static_cast<float>(m_bgTexture.getSize().y);
        m_bgSprite.setScale(sx, sy);
    }

    // ── Titre ──────────────────────────────────────────────────────
    m_title.setFont(m_font);
    m_title.setString("Defend the Castle");
    m_title.setCharacterSize(72);
    m_title.setFillColor(sf::Color::Black);
    m_title.setStyle(sf::Text::Bold);

    // Centrage horizontal, quart supérieur de l'écran
    sf::FloatRect tb = m_title.getLocalBounds();
    float titleX = (m_window.getSize().x - tb.width)  / 2.f - tb.left;
    float titleY = m_window.getSize().y * 0.22f;
    m_title.setPosition(titleX, titleY);

    // ── Icône settings (texture optionnelle) ───────────────────────
    if (m_settingsTexture.loadFromFile("assets/sprites/settings_icon.png")) {
        m_settingsSprite.setTexture(m_settingsTexture);
        m_settingsSprite.setPosition(12.f, 12.f);
        float scale = 48.f / static_cast<float>(m_settingsTexture.getSize().x);
        m_settingsSprite.setScale(scale, scale);
        m_hasSettingsTexture = true;
    }
}

// ═══════════════════════════════════════════════════════════════════
//  Construction des boutons
// ═══════════════════════════════════════════════════════════════════

void MainMenu::buildButtons() {
    const float winW  = static_cast<float>(m_window.getSize().x);
    const float winH  = static_cast<float>(m_window.getSize().y);
    const float btnW  = 220.f;
    const float btnH  = 55.f;
    const float cx    = winW / 2.f;

    // Couleurs communes
    sf::Color normalDark  (30,  30,  30,  210);
    sf::Color hoverDark   (60,  60,  60,  230);
    sf::Color normalRed   (200, 30,  30,  230);
    sf::Color hoverRed    (230, 60,  60,  255);

    m_buttons.push_back(makeButton("New Game",   cx, winH * 0.62f, btnW, btnH,
                                   MenuAction::NewGame,    normalDark, hoverDark));
    m_buttons.push_back(makeButton("Scoreboard", cx, winH * 0.73f, btnW, btnH,
                                   MenuAction::Scoreboard, normalDark, hoverDark));
    m_buttons.push_back(makeButton("EXIT",       cx, winH * 0.84f, btnW, btnH,
                                   MenuAction::Exit,       normalRed,  hoverRed));
}

// ═══════════════════════════════════════════════════════════════════
//  Icône Settings (coin haut-gauche)
// ═══════════════════════════════════════════════════════════════════

void MainMenu::buildSettingsIcon() {
    m_settingsIcon.setSize({ 48.f, 48.f });
    m_settingsIcon.setPosition(12.f, 12.f);
    m_settingsIcon.setFillColor(sf::Color(50, 50, 50, 180));
    m_settingsIcon.setOutlineColor(sf::Color(200, 200, 200, 150));
    m_settingsIcon.setOutlineThickness(1.5f);
}

// ═══════════════════════════════════════════════════════════════════
//  Utilitaire — fabrique un bouton centré
// ═══════════════════════════════════════════════════════════════════

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

    // Forme
    btn.shape.setSize({ width, height });
    btn.shape.setOrigin(width / 2.f, height / 2.f);
    btn.shape.setPosition(cx, cy);
    btn.shape.setFillColor(normal);

    // Texte
    btn.label.setFont(m_font);
    btn.label.setString(text);
    btn.label.setCharacterSize(24);
    btn.label.setFillColor(sf::Color::White);
    btn.label.setStyle(sf::Text::Regular);

    sf::FloatRect lb = btn.label.getLocalBounds();
    btn.label.setOrigin(lb.left + lb.width  / 2.f,
                        lb.top  + lb.height / 2.f);
    btn.label.setPosition(cx, cy);

    return btn;
}

// ═══════════════════════════════════════════════════════════════════
//  Boucle principale
// ═══════════════════════════════════════════════════════════════════

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

// ═══════════════════════════════════════════════════════════════════
//  Gestion des événements
// ═══════════════════════════════════════════════════════════════════

void MainMenu::handleEvents(MenuAction& result) {
    sf::Event event;
    while (m_window.pollEvent(event)) {

        if (event.type == sf::Event::Closed) {
            m_window.close();
            result    = MenuAction::Exit;
            m_running = false;
            return;
        }

        if (event.type == sf::Event::MouseButtonReleased &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f pos = m_window.mapPixelToCoords(
                { event.mouseButton.x, event.mouseButton.y });

            // Boutons principaux
            for (auto& btn : m_buttons) {
                if (btn.contains(pos)) {
                    result    = btn.action;
                    m_running = false;
                    return;
                }
            }

            // Icône settings
            if (m_settingsIcon.getGlobalBounds().contains(pos)) {
                result    = MenuAction::Settings;
                m_running = false;
                return;
            }
        }

        // Touche Échap → quitter
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape)
        {
            result    = MenuAction::Exit;
            m_running = false;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
//  Update (hover)
// ═══════════════════════════════════════════════════════════════════

void MainMenu::update(sf::Vector2f mousePos) {
    for (auto& btn : m_buttons) {
        btn.setHovered(btn.contains(mousePos));
    }

    // Hover settings icon
    bool settingsHover = m_settingsIcon.getGlobalBounds().contains(mousePos);
    m_settingsIcon.setFillColor(settingsHover
        ? sf::Color(90, 90, 90, 200)
        : sf::Color(50, 50, 50, 180));
}


//  Rendu

void MainMenu::render() {
    m_window.clear(sf::Color(100, 149, 237)); // fallback si pas de bg

    // Background
    m_window.draw(m_bgSprite);

    // Titre
    m_window.draw(m_title);

    // Boutons
    for (auto& btn : m_buttons) {
        m_window.draw(btn.shape);
        m_window.draw(btn.label);
    }

    // Icône settings
    if (m_hasSettingsTexture) {
        m_window.draw(m_settingsSprite);
    } else {
        m_window.draw(m_settingsIcon);

        // Engrenage minimaliste dessiné avec des cercles si pas de texture
        sf::CircleShape gear(10.f);
        gear.setOrigin(10.f, 10.f);
        gear.setPosition(36.f, 36.f);
        gear.setFillColor(sf::Color(200, 200, 200, 200));
        m_window.draw(gear);

        sf::CircleShape hole(4.f);
        hole.setOrigin(4.f, 4.f);
        hole.setPosition(36.f, 36.f);
        hole.setFillColor(sf::Color(50, 50, 50, 180));
        m_window.draw(hole);
    }

    m_window.display();
}