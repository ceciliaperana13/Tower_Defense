#include "MainMenu.hpp"
#include <iostream>
#include <stdexcept>

// ─── MenuButton helpers 
bool MenuButton::contains(sf::Vector2f point) const {
    return shape.getGlobalBounds().contains(point);
}

void MenuButton::setHovered(bool hovered) {
    shape.setFillColor(hovered ? sf::Color(180, 180, 180, 255)
                               : sf::Color(255, 255, 255, 255));
}

// ─── Constructeur 
MainMenu::MainMenu(sf::RenderWindow& window)
    : m_window(window)
{
    loadAssets();
    buildButtons();
    loadCharacters();
}

// ─── loadAssets 
void MainMenu::loadAssets() {
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        throw std::runtime_error("MainMenu: impossible de charger la police.");

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

    m_title.emplace(m_font, "Defend the Castle", 72u);
    m_title->setFillColor(sf::Color::White);
    m_title->setStyle(sf::Text::Bold);
    sf::FloatRect tb = m_title->getLocalBounds();
    float titleX = (static_cast<float>(m_window.getSize().x) - tb.size.x) / 2.f - tb.position.x;
    float titleY = static_cast<float>(m_window.getSize().y) * 0.10f;
    m_title->setPosition({ titleX, titleY });

    if (!m_settingsTexture.loadFromFile("../assets/sprites/setting.png")) {
        std::cerr << "[MainMenu] setting.png introuvable.\n";
    } else {
        m_settingsSprite.emplace(m_settingsTexture);
        float scale = 48.f / static_cast<float>(m_settingsTexture.getSize().x);
        m_settingsSprite->setScale({ scale, scale });
        m_settingsSprite->setPosition({ 12.f, 12.f });
    }
}

// ─── buildButtons 
void MainMenu::buildButtons() {
    const float winW = static_cast<float>(m_window.getSize().x);
    const float winH = static_cast<float>(m_window.getSize().y);
    const float btnW = 300.f;
    const float btnH = 80.f;
    const float cx   = winW / 2.f;

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

// ─── makeButton 
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

// ─── loadCharacters 
void MainMenu::loadCharacters() {
    const float winW = static_cast<float>(m_window.getSize().x);
    const float winH = static_cast<float>(m_window.getSize().y);

    struct CharDef {
        std::string f1, f2;
        float x, y, scaleX, scaleY, speed;
        bool  flipped;
        float animSpeed;
    };

    // Plusieurs gobelins et ogres des deux côtés, à des hauteurs légèrement différentes
    // pour donner de la profondeur. Les flippés marchent vers la gauche.
    const std::vector<CharDef> defs = {
        // ── Gobelins gauche → droite ──
        { "../assets/sprites/enemies/goblin1.png",
          "../assets/sprites/enemies/goblin2.png",
          -60.f, winH * 0.78f,  2.2f, 2.2f,  95.f, false, 0.28f },

        { "../assets/sprites/enemies/goblin1.png",
          "../assets/sprites/enemies/goblin2.png",
          -200.f, winH * 0.83f, 1.8f, 1.8f,  80.f, false, 0.32f },

        // ── Gobelins droite → gauche ──
        { "../assets/sprites/enemies/goblin1.png",
          "../assets/sprites/enemies/goblin2.png",
          winW + 60.f,  winH * 0.76f, 2.2f, 2.2f, -90.f, true, 0.28f },

        { "../assets/sprites/enemies/goblin1.png",
          "../assets/sprites/enemies/goblin2.png",
          winW + 220.f, winH * 0.81f, 1.8f, 1.8f, -75.f, true, 0.30f },

        // ── Ogres gauche → droite ──
        { "../assets/sprites/enemies/ogre_boss1.png",
          "../assets/sprites/enemies/ogre_boss2.png",
          -110.f, winH * 0.72f, 3.2f, 3.2f,  52.f, false, 0.40f },

        // ── Ogres droite → gauche ──
        { "../assets/sprites/enemies/ogre_boss1.png",
          "../assets/sprites/enemies/ogre_boss2.png",
          winW + 110.f, winH * 0.70f, 3.2f, 3.2f, -48.f, true, 0.42f },
    };

    for (const auto& d : defs) {
        WalkingCharacter c;
        c.speed     = d.speed;
        c.flipped   = d.flipped;
        c.animSpeed = d.animSpeed;

        if (!c.frames[0].loadFromFile(d.f1))
            std::cerr << "[MainMenu] Introuvable : " << d.f1 << "\n";
        if (!c.frames[1].loadFromFile(d.f2))
            std::cerr << "[MainMenu] Introuvable : " << d.f2 << "\n";

        float w = static_cast<float>(c.frames[0].getSize().x) * d.scaleX;
        float h = static_cast<float>(c.frames[0].getSize().y) * d.scaleY;

        c.shape.setSize({ w, h });
        c.shape.setOrigin({ w / 2.f, h });          // pivot bas-centre
        c.shape.setPosition({ d.x, d.y });
        c.shape.setTexture(&c.frames[0]);

        // Flip horizontal : scale négatif en X, on compense l'origine
        if (d.flipped)
            c.shape.setScale({ -1.f, 1.f });

        m_characters.push_back(std::move(c));
    }
}

// ─── updateCharacters 
void MainMenu::updateCharacters(float dt) {
    const float winW = static_cast<float>(m_window.getSize().x);

    for (auto& c : m_characters) {
        // Déplacement
        sf::Vector2f pos = c.shape.getPosition();
        pos.x += c.speed * dt;

        // Boucle infinie : réapparition côté opposé
        float halfW = c.shape.getSize().x / 2.f;
        if (c.speed > 0.f && pos.x - halfW > winW)
            pos.x = -halfW;
        else if (c.speed < 0.f && pos.x + halfW < 0.f)
            pos.x = winW + halfW;

        c.shape.setPosition(pos);

        // Alternance des frames
        c.animTimer += dt;
        if (c.animTimer >= c.animSpeed) {
            c.animTimer    = 0.f;
            c.currentFrame = 1 - c.currentFrame;
            c.shape.setTexture(&c.frames[c.currentFrame]);
        }
    }
}

// ─── renderCharacters 
void MainMenu::renderCharacters() {
    for (auto& c : m_characters)
        m_window.draw(c.shape);
}

// ─── run 
MenuAction MainMenu::run() {
    MenuAction result = MenuAction::None;
    m_running = true;
    m_clock.restart();

    while (m_running && m_window.isOpen()) {
        float dt = m_clock.restart().asSeconds();
        // Clamp dt pour éviter les sauts si la fenêtre est déplacée / mise en pause
        if (dt > 0.1f) dt = 0.1f;

        sf::Vector2f mouse = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
        handleEvents(result);
        update(mouse, dt);
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

void MainMenu::update(sf::Vector2f mousePos, float dt) {
    for (auto& btn : m_buttons)
        btn.setHovered(btn.contains(mousePos));

    updateCharacters(dt);
}

// ─── render 
void MainMenu::render() {
    m_window.clear(sf::Color(20, 20, 40));

    // 1. Background
    if (m_bgSprite.has_value())
        m_window.draw(*m_bgSprite);

    // 2. Personnages animés (derrière les boutons)
    renderCharacters();

    // 3. Titre + ombre
    if (m_title.has_value()) {
        sf::Text shadow = *m_title;
        shadow.setFillColor(sf::Color(0, 0, 0, 150));
        shadow.move({ 3.f, 3.f });
        m_window.draw(shadow);
        m_window.draw(*m_title);
    }

    // 4. Boutons
    for (auto& btn : m_buttons)
        m_window.draw(btn.shape);

    // 5. Icône settings
    if (m_settingsSprite.has_value())
        m_window.draw(*m_settingsSprite);

    m_window.display();
}