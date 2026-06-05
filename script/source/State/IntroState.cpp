#include "State/IntroState.hpp"
#include "State/GameState.hpp"
#include "Game.hpp"
#include <cmath>
#include <iostream>

IntroState::IntroState(sf::RenderWindow& window)
    : m_window(window)
{
    const float W = float(WIN_W);
    const float H = float(WIN_H);

    // ── Police ───────────────────────────────────────────────
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[IntroState] Font introuvable\n";

    // ── Background ───────────────────────────────────────────
    if (!m_bgTex.loadFromFile("../assets/sprites/background.jpg")) {
        std::cerr << "[IntroState] background.jpg introuvable\n";
    } else {
        m_bgSprite.emplace(m_bgTex);
        m_bgSprite->setScale({
            W / float(m_bgTex.getSize().x),
            H / float(m_bgTex.getSize().y)
        });
    }

    // ── Château ──────────────────────────────────────────────
    if (!m_castleTex.loadFromFile("../assets/sprites/big_buildings/castle.png")) {
        std::cerr << "[IntroState] castle.png introuvable\n";
    } else {
        m_castleSprite.emplace(m_castleTex);
        float scale = 220.f / float(m_castleTex.getSize().x);
        m_castleSprite->setScale({ scale, scale });
        sf::Vector2f pos { W * 0.72f, H * 0.38f };
        m_castleSprite->setPosition(pos);
        m_castleBasePos = pos;
    }

    // ── Ogre ─────────────────────────────────────────────────
    if (!m_ogreTex[0].loadFromFile("../assets/sprites/enemies/ogre_boss1.png"))
        std::cerr << "[IntroState] ogre_boss1.png introuvable\n";
    if (!m_ogreTex[1].loadFromFile("../assets/sprites/enemies/ogre_boss2.png"))
        std::cerr << "[IntroState] ogre_boss2.png introuvable\n";

    m_ogreSprite.emplace(m_ogreTex[0]);
    float oScale = 200.f / float(m_ogreTex[0].getSize().x);
    m_ogreSprite->setScale({ oScale, oScale });

    // L'ogre part de la gauche, atteint le château en 5s sur 8s total
    m_ogreX      = -220.f;
    float target = W * 0.72f - 240.f;
    m_ogreSpeed  = (target - m_ogreX) / 5.f;
    m_ogreSprite->setPosition({ m_ogreX, H * 0.42f });

    // ── Titre ────────────────────────────────────────────────
    m_titleText.emplace(m_font, "L'assaut commence !", 52u);
    m_titleText->setFillColor(sf::Color(255, 80, 30));
    m_titleText->setStyle(sf::Text::Bold);
    sf::FloatRect tb = m_titleText->getLocalBounds();
    m_titleText->setPosition({
        (W - tb.size.x) / 2.f - tb.position.x,
        H * 0.07f
    });

    // ── Skip ─────────────────────────────────────────────────
    m_skipText.emplace(m_font, "Appuyez sur ESPACE pour passer", 15u);
    m_skipText->setFillColor(sf::Color(160, 160, 160));
    sf::FloatRect sk = m_skipText->getLocalBounds();
    m_skipText->setPosition({
        (W - sk.size.x) / 2.f - sk.position.x,
        H - 32.f
    });

    // ── Barre de chargement ──────────────────────────────────
    float barW = W * 0.55f;
    float barH = 24.f;
    float barX = (W - barW) / 2.f;
    float barY = H - 72.f;

    m_barBg.setSize({ barW, barH });
    m_barBg.setPosition({ barX, barY });
    m_barBg.setFillColor(sf::Color(20, 20, 20, 210));
    m_barBg.setOutlineColor(sf::Color(200, 80, 30));
    m_barBg.setOutlineThickness(2.f);

    m_barFill.setSize({ 0.f, barH });
    m_barFill.setPosition({ barX, barY });
    m_barFill.setFillColor(sf::Color(220, 60, 20));

    m_barLabel.emplace(m_font, "Preparation de l'assaut...", 13u);
    m_barLabel->setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect lb = m_barLabel->getLocalBounds();
    m_barLabel->setPosition({
        (W - lb.size.x) / 2.f - lb.position.x,
        barY - 22.f
    });
}

// ─── Events ──────────────────────────────────────────────────

void IntroState::handleEvents(const sf::Event& event) {
    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Space ||
            kp->code == sf::Keyboard::Key::Enter ||
            kp->code == sf::Keyboard::Key::Escape)
            goToGame();
    }
    if (event.getIf<sf::Event::MouseButtonReleased>())
        goToGame();
}

// ─── Update ──────────────────────────────────────────────────

void IntroState::update(float dt) {
    // Fade out final → lance le jeu
    if (m_fadingOut) {
        m_fadeAlpha += dt * 255.f / 0.6f;
        if (m_fadeAlpha >= 255.f) {
            Game::getInstance().getStateManager()
                .change(std::make_unique<GameState>(m_window));
        }
        return;
    }

    m_elapsed += dt;

    // Barre de progression
    float ratio = std::min(m_elapsed / DURATION, 1.f);
    float barW  = float(WIN_W) * 0.55f;
    m_barFill.setSize({ barW * ratio, 24.f });

    // Déclenchement fade out à la fin
    if (m_elapsed >= DURATION && !m_fadingOut)
        m_fadingOut = true;

    // Animation ogre
    m_animTimer += dt;
    if (m_animTimer >= ANIM_SPEED) {
        m_animTimer = 0.f;
        m_animFrame = 1 - m_animFrame;
        if (m_ogreSprite)
            m_ogreSprite->setTexture(m_ogreTex[m_animFrame]);
    }

    // Mouvement ogre
    float W      = float(WIN_W);
    float target = W * 0.72f - 240.f;
    if (m_ogreX < target) {
        m_ogreX += m_ogreSpeed * dt;
        if (m_ogreSprite)
            m_ogreSprite->setPosition({ m_ogreX, float(WIN_H) * 0.42f });
    }

    // Impact : shake + flash
    if (m_ogreX >= target - 2.f && !m_shaking) {
        m_shaking     = true;
        m_shakeTimer  = 0.f;
        m_flashActive = true;
        m_flashTimer  = 0.f;
    }

    if (m_shaking) {
        m_shakeTimer += dt;
        if (m_shakeTimer < 1.2f) {
            float intensity = 7.f * (1.f - m_shakeTimer / 1.2f);
            float ox = (std::fmod(m_shakeTimer * 55.f, 2.f) < 1.f ?  1.f : -1.f) * intensity;
            float oy = (std::fmod(m_shakeTimer * 43.f, 2.f) < 1.f ?  1.f : -1.f) * intensity * 0.4f;
            if (m_castleSprite)
                m_castleSprite->setPosition({ m_castleBasePos.x + ox, m_castleBasePos.y + oy });
        } else {
            m_shaking = false;
            if (m_castleSprite)
                m_castleSprite->setPosition(m_castleBasePos);
        }
    }

    if (m_flashActive) {
        m_flashTimer += dt;
        if (m_flashTimer > 0.35f)
            m_flashActive = false;
    }

    // Titre : pulsation couleur
    if (m_titleText) {
        uint8_t r = uint8_t(200 + 55 * std::abs(std::sin(m_elapsed * 3.f)));
        m_titleText->setFillColor(sf::Color(r, 60, 20));
    }
}

// ─── Render ──────────────────────────────────────────────────

void IntroState::render(sf::RenderWindow& window) {
    // Background
    if (m_bgSprite) window.draw(*m_bgSprite);

    // Overlay sombre
    sf::RectangleShape overlay{sf::Vector2f(float(WIN_W), float(WIN_H))};
    overlay.setFillColor(sf::Color(0, 0, 0, 120));
    window.draw(overlay);

    // Château
    if (m_castleSprite) window.draw(*m_castleSprite);

    // Ogre
    if (m_ogreSprite) window.draw(*m_ogreSprite);

    // Flash impact
    if (m_flashActive) {
        float alpha = 200.f * (1.f - m_flashTimer / 0.35f);
        sf::RectangleShape flash{sf::Vector2f(float(WIN_W), float(WIN_H))};
        flash.setFillColor(sf::Color(255, 100, 0, uint8_t(alpha)));
        window.draw(flash);
    }

    // Titre
    if (m_titleText) {
        sf::Text shadow = *m_titleText;
        shadow.setFillColor(sf::Color(0, 0, 0, 150));
        shadow.move({ 3.f, 3.f });
        window.draw(shadow);
        window.draw(*m_titleText);
    }

    // Barre
    window.draw(m_barBg);
    window.draw(m_barFill);
    if (m_barLabel) window.draw(*m_barLabel);

    // Skip
    if (m_skipText) window.draw(*m_skipText);

    // Fade out
    if (m_fadingOut) {
        sf::RectangleShape fade{sf::Vector2f(float(WIN_W), float(WIN_H))};
        fade.setFillColor(sf::Color(0, 0, 0, uint8_t(std::min(m_fadeAlpha, 255.f))));
        window.draw(fade);
    }
}

// ─── Transition ──────────────────────────────────────────────

void IntroState::goToGame() {
    m_fadingOut = true;
    m_fadeAlpha = 0.f;
}