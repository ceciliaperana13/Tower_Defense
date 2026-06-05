#include "State/SplashState.hpp"
#include "State/MenuState.hpp"
#include "Game.hpp"
#include <cmath>
#include <iostream>

SplashState::SplashState(sf::RenderWindow& window)
    : m_window(window)
{
    const float W = float(WIN_W);
    const float H = float(WIN_H);

    // ── Background ───────────────────────────────────────────
    if (!m_bgTex.loadFromFile("../assets/sprites/background.jpg")) {
        std::cerr << "[SplashState] background.jpg introuvable\n";
    } else {
        m_bgSprite.emplace(m_bgTex);
        float sx = W / float(m_bgTex.getSize().x);
        float sy = H / float(m_bgTex.getSize().y);
        m_bgSprite->setScale({ sx, sy });
    }

    // ── Château ──────────────────────────────────────────────
    if (!m_castleTex.loadFromFile("../assets/sprites/big_buildings/castle.png")) {
        std::cerr << "[SplashState] castle.png introuvable\n";
    } else {
        m_castleSprite.emplace(m_castleTex);
        float scale = 200.f / float(m_castleTex.getSize().x);
        m_castleSprite->setScale({ scale, scale });
        float cx = W * 0.75f;
        float cy = H * 0.45f;
        m_castleSprite->setPosition({ cx, cy });
        m_castleBasePos = { cx, cy };
    }

    // ── Ogre ─────────────────────────────────────────────────
    if (!m_ogreTex[0].loadFromFile("../assets/sprites/enemies/ogre_boss1.png"))
        std::cerr << "[SplashState] ogre_boss1.png introuvable\n";
    if (!m_ogreTex[1].loadFromFile("../assets/sprites/enemies/ogre_boss2.png"))
        std::cerr << "[SplashState] ogre_boss2.png introuvable\n";

    m_ogreSprite.emplace(m_ogreTex[0]);
    float oScale = 180.f / float(m_ogreTex[0].getSize().x);
    m_ogreSprite->setScale({ oScale, oScale });

    // L'ogre part de la gauche et doit atteindre le château en ~15 secondes
    m_ogreX     = -200.f;
    float target = W * 0.75f - 220.f;
    m_ogreSpeed  = (target - m_ogreX) / 15.f;

    m_ogreSprite->setPosition({ m_ogreX, H * 0.48f });

    // ── Textes ───────────────────────────────────────────────
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        std::cerr << "[SplashState] Font introuvable\n";

    m_titleText.emplace(m_font, "Defend the Castle", 64u);
    m_titleText->setFillColor(sf::Color(255, 200, 50));
    m_titleText->setStyle(sf::Text::Bold);
    sf::FloatRect tb = m_titleText->getLocalBounds();
    m_titleText->setPosition({
        (W - tb.size.x) / 2.f - tb.position.x,
        H * 0.06f
    });

    m_skipText.emplace(m_font, "Appuyez sur ESPACE pour passer", 16u);
    m_skipText->setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect sb = m_skipText->getLocalBounds();
    m_skipText->setPosition({
        (W - sb.size.x) / 2.f - sb.position.x,
        H - 36.f
    });

    // ── Barre de progression ─────────────────────────────────
    float barW = W * 0.6f;
    float barH = 22.f;
    float barX = (W - barW) / 2.f;
    float barY = H - 70.f;

    m_barBg.setSize({ barW, barH });
    m_barBg.setPosition({ barX, barY });
    m_barBg.setFillColor(sf::Color(30, 30, 30, 200));
    m_barBg.setOutlineColor(sf::Color(180, 140, 60));
    m_barBg.setOutlineThickness(2.f);

    m_barFill.setSize({ 0.f, barH });
    m_barFill.setPosition({ barX, barY });
    m_barFill.setFillColor(sf::Color(200, 80, 30));

    m_barLabel.emplace(m_font, "Chargement...", 14u);
    m_barLabel->setFillColor(sf::Color::White);
    sf::FloatRect lb = m_barLabel->getLocalBounds();
    m_barLabel->setPosition({
        (W - lb.size.x) / 2.f - lb.position.x,
        barY - 24.f
    });
}

void SplashState::onEnter() {}

void SplashState::handleEvents(const sf::Event& event) {
    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Space ||
            kp->code == sf::Keyboard::Key::Enter ||
            kp->code == sf::Keyboard::Key::Escape)
        {
            goToMenu();
        }
    }
    if (event.getIf<sf::Event::MouseButtonReleased>())
        goToMenu();
}

void SplashState::update(float dt) {
    m_elapsed += dt;

    if (m_elapsed >= DURATION) {
        goToMenu();
        return;
    }

    const float W = float(WIN_W);
    const float H = float(WIN_H);

    // ── Barre de progression ─────────────────────────────────
    float ratio = m_elapsed / DURATION;
    float barW  = W * 0.6f;
    m_barFill.setSize({ barW * ratio, 22.f });

    // ── Animation ogre ───────────────────────────────────────
    m_animTimer += dt;
    if (m_animTimer >= ANIM_SPEED) {
        m_animTimer = 0.f;
        m_animFrame = 1 - m_animFrame;
        if (m_ogreSprite)
            m_ogreSprite->setTexture(m_ogreTex[m_animFrame]);
    }

    // ── Mouvement ogre ───────────────────────────────────────
    float target = W * 0.75f - 220.f;
    if (m_ogreX < target) {
        m_ogreX += m_ogreSpeed * dt;
        if (m_ogreSprite)
            m_ogreSprite->setPosition({ m_ogreX, H * 0.48f });
    }

    // ── Impact : shake + flash à l'arrivée ───────────────────
    bool atTarget = m_ogreX >= target - 2.f;

    if (atTarget && !m_shaking) {
        m_shaking    = true;
        m_shakeTimer = 0.f;
        m_flashActive = true;
        m_flashTimer  = 0.f;
    }

    if (m_shaking) {
        m_shakeTimer += dt;
        float intensity = 6.f * (1.f - m_shakeTimer / 1.5f);
        if (m_shakeTimer < 1.5f) {
            float ox = (std::fmod(m_shakeTimer * 60.f, 2.f) < 1.f ? 1.f : -1.f) * intensity;
            float oy = (std::fmod(m_shakeTimer * 47.f, 2.f) < 1.f ? 1.f : -1.f) * intensity * 0.5f;
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
        if (m_flashTimer > 0.3f)
            m_flashActive = false;
    }

    // ── Titre : effet pulsation ───────────────────────────────
    if (m_titleText) {
        float pulse = 1.f + 0.04f * std::sin(m_elapsed * 2.f);
        // on recentre le titre avec la pulsation
        sf::FloatRect tb = m_titleText->getLocalBounds();
        float W2 = float(WIN_W);
        m_titleText->setPosition({
            (W2 - tb.size.x * pulse) / 2.f - tb.position.x,
            float(WIN_H) * 0.06f
        });
        // couleur dorée qui pulse légèrement
        uint8_t r = uint8_t(200 + 55 * std::abs(std::sin(m_elapsed * 1.5f)));
        m_titleText->setFillColor(sf::Color(r, 180, 40));
    }
}

void SplashState::render(sf::RenderWindow& window) {
    // Background
    if (m_bgSprite) window.draw(*m_bgSprite);

    // Overlay sombre pour ambiance cinématique
    sf::RectangleShape overlay{sf::Vector2f(float(WIN_W), float(WIN_H))};
    overlay.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(overlay);

    // Château
    if (m_castleSprite) window.draw(*m_castleSprite);

    // Ogre
    if (m_ogreSprite) window.draw(*m_ogreSprite);

    // Flash d'impact
    if (m_flashActive) {
        float alpha = 180.f * (1.f - m_flashTimer / 0.3f);
        sf::RectangleShape flash{sf::Vector2f(float(WIN_W), float(WIN_H))};
        flash.setFillColor(sf::Color(255, 120, 0, uint8_t(alpha)));
        window.draw(flash);
    }

    // Titre
    if (m_titleText) {
        sf::Text shadow = *m_titleText;
        shadow.setFillColor(sf::Color(0, 0, 0, 160));
        shadow.move({ 3.f, 3.f });
        window.draw(shadow);
        window.draw(*m_titleText);
    }

    // Barre de progression
    window.draw(m_barBg);
    window.draw(m_barFill);
    if (m_barLabel) window.draw(*m_barLabel);

    // Skip hint
    if (m_skipText) window.draw(*m_skipText);
}

void SplashState::goToMenu() {
    Game::getInstance().getStateManager()
        .change(std::make_unique<MenuState>(m_window));
}