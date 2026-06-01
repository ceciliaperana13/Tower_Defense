#include "SettingsMenu.hpp"
#include <iostream>
//languages
const std::vector<std::string> GameSettings::LANGUAGES = {
    "Francais", "English", "Espanol"
};

SettingsMenu::SettingsMenu(sf::RenderWindow& window, GameSettings& settings)
    : m_window(window), m_settings(settings)
{
    if (!m_font.openFromFile("C:/Windows/Fonts/arialbd.ttf"))
        throw std::runtime_error("SettingsMenu: police introuvable.");
    buildUI();
}
// ─── buildUI
void SettingsMenu::buildUI() {
    const float winW = static_cast<float>(m_window.getSize().x);
    const float winH = static_cast<float>(m_window.getSize().y);

    m_panelW = 460.f;
    m_panelH = 360.f;
    m_panelX = (winW - m_panelW) / 2.f;
    m_panelY = (winH - m_panelH) / 2.f;

    m_panel.setSize({ m_panelW, m_panelH });
    m_panel.setPosition({ m_panelX, m_panelY });
    m_panel.setFillColor(sf::Color(15, 15, 25, 240));
    m_panel.setOutlineColor(sf::Color(180, 140, 60));
    m_panel.setOutlineThickness(2.f);

    m_titleText.emplace(m_font, "Settings", 30u);
    m_titleText->setFillColor(sf::Color(220, 180, 80));
    m_titleText->setStyle(sf::Text::Bold);
    sf::FloatRect tb = m_titleText->getLocalBounds();
    m_titleText->setPosition({
        m_panelX + (m_panelW - tb.size.x) / 2.f - tb.position.x,
        m_panelY + 16.f
    });

    m_closeBtn.setSize({ 30.f, 30.f });
    m_closeBtn.setPosition({ m_panelX + m_panelW - 40.f, m_panelY + 10.f });
    m_closeBtn.setFillColor(sf::Color(180, 40, 40));
    m_closeBtn.setOutlineColor(sf::Color(255, 100, 100));
    m_closeBtn.setOutlineThickness(1.f);

    m_closeLabel.emplace(m_font, "X", 18u);
    m_closeLabel->setFillColor(sf::Color::White);
    sf::FloatRect cb = m_closeLabel->getLocalBounds();
    m_closeLabel->setPosition({
        m_closeBtn.getPosition().x + (30.f - cb.size.x) / 2.f - cb.position.x,
        m_closeBtn.getPosition().y + (30.f - cb.size.y) / 2.f - cb.position.y
    });

    float trackW = 200.f;
    float trackX = m_panelX + m_panelW - trackW - 40.f;

    m_musicTrack = { {trackX, m_panelY + 90.f},  {trackW, 14.f} };
    m_sfxTrack   = { {trackX, m_panelY + 160.f}, {trackW, 14.f} };

    // Checkbox plein ecran alignee a droite
    m_fullscreenBox = { {m_panelX + m_panelW - 54.f, m_panelY + 228.f}, {24.f, 24.f} };

    // Langue
    float rowY = m_panelY + 295.f;
    m_langLeftBtn  = { {trackX,               rowY}, {28.f, 28.f} };
    m_langRightBtn = { {trackX + trackW - 28.f, rowY}, {28.f, 28.f} };
}
// ─── run
void SettingsMenu::run() {
    m_running = true;
    while (m_running && m_window.isOpen()) {
        sf::Vector2f mouse = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
        handleEvents();
        update(mouse);
        render();
    }
}
// ─── handleEvents
void SettingsMenu::handleEvents() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {

        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            m_running = false;
            return;
        }

        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Escape) {
                m_running = false;
                return;
            }
        }

        if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mb->button == sf::Mouse::Button::Left) {
                sf::Vector2f pos = m_window.mapPixelToCoords({ mb->position.x, mb->position.y });

                if (m_closeBtn.getGlobalBounds().contains(pos)) {
                    m_running = false; return;
                }
                if (m_musicTrack.contains(pos)) { m_draggingMusic = true; onMusicSlider(pos.x); }
                if (m_sfxTrack.contains(pos))   { m_draggingSfx   = true; onSfxSlider(pos.x);   }
                if (m_fullscreenBox.contains(pos)) onFullscreenToggle();
                if (m_langLeftBtn.contains(pos))   onLanguagePrev();
                if (m_langRightBtn.contains(pos))  onLanguageNext();
            }
        }

        if (const auto* mb = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mb->button == sf::Mouse::Button::Left) {
                m_draggingMusic = false;
                m_draggingSfx   = false;
            }
        }
    }
}

void SettingsMenu::update(sf::Vector2f mouse) {
    if (m_draggingMusic) onMusicSlider(mouse.x);
    if (m_draggingSfx)   onSfxSlider(mouse.x);
}

void SettingsMenu::onMusicSlider(float mx) {
    float rel = (mx - m_musicTrack.position.x) / m_musicTrack.size.x;
    m_settings.musicVolume = std::max(0.f, std::min(100.f, rel * 100.f));
}

void SettingsMenu::onSfxSlider(float mx) {
    float rel = (mx - m_sfxTrack.position.x) / m_sfxTrack.size.x;
    m_settings.sfxVolume = std::max(0.f, std::min(100.f, rel * 100.f));
}

void SettingsMenu::onFullscreenToggle() {
    m_settings.fullscreen = !m_settings.fullscreen;

    sf::VideoMode mode = m_settings.fullscreen
        ? sf::VideoMode::getDesktopMode()
        : sf::VideoMode({ 1080u, 720u });

    m_window.create(mode, "Defend the Castle",
        m_settings.fullscreen ? sf::State::Fullscreen : sf::State::Windowed);
    m_window.setFramerateLimit(60);

    // Recalcule les positions du panneau apres recreation
    buildUI();
}

void SettingsMenu::onLanguageNext() {
    m_settings.languageIdx =
        (m_settings.languageIdx + 1) % (int)GameSettings::LANGUAGES.size();
}

void SettingsMenu::onLanguagePrev() {
    int n = (int)GameSettings::LANGUAGES.size();
    m_settings.languageIdx = (m_settings.languageIdx + n - 1) % n;
}

// Helper local
static void drawLabel(sf::RenderWindow& w, sf::Font& f,
                      const std::string& txt, sf::Vector2f pos) {
    sf::Text t(f, txt, 18u);
    t.setFillColor(sf::Color(210, 210, 210));
    t.setPosition(pos);
    w.draw(t);
}

static void drawSliderRow(sf::RenderWindow& w, sf::Font& f,
                          const std::string& label, float value,
                          sf::FloatRect track)
{
    // Label
    drawLabel(w, f, label, { track.position.x - 220.f, track.position.y - 2.f });

    // Track fond
    sf::RectangleShape bg({ track.size.x, track.size.y });
    bg.setPosition(track.position);
    bg.setFillColor(sf::Color(55, 55, 55));
    bg.setOutlineColor(sf::Color(100, 100, 100));
    bg.setOutlineThickness(1.f);
    w.draw(bg);

    // Fill
    float fillW = track.size.x * value / 100.f;
    sf::RectangleShape fill({ fillW, track.size.y });
    fill.setPosition(track.position);
    fill.setFillColor(sf::Color(220, 175, 60));
    w.draw(fill);

    // Knob
    sf::CircleShape knob(8.f);
    knob.setOrigin({ 8.f, 8.f });
    knob.setPosition({ track.position.x + fillW, track.position.y + track.size.y / 2.f });
    knob.setFillColor(sf::Color(240, 240, 240));
    knob.setOutlineColor(sf::Color(180, 140, 60));
    knob.setOutlineThickness(1.5f);
    w.draw(knob);

    // Valeur
    sf::Text val(f, std::to_string((int)value), 15u);
    val.setFillColor(sf::Color(180, 180, 180));
    val.setPosition({ track.position.x + track.size.x + 10.f, track.position.y });
    w.draw(val);
}

void SettingsMenu::render() {
    // Overlay
    sf::RectangleShape overlay({
        static_cast<float>(m_window.getSize().x),
        static_cast<float>(m_window.getSize().y)
    });
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    m_window.draw(overlay);

    // Panneau
    m_window.draw(m_panel);

    // Titre
    if (m_titleText) m_window.draw(*m_titleText);

    // Separateur
    sf::RectangleShape sep({ m_panelW - 40.f, 1.f });
    sep.setPosition({ m_panelX + 20.f, m_panelY + 60.f });
    sep.setFillColor(sf::Color(180, 140, 60, 100));
    m_window.draw(sep);

    // Bouton fermer
    m_window.draw(m_closeBtn);
    if (m_closeLabel) m_window.draw(*m_closeLabel);

    float labelX = m_panelX + 30.f;

    // Sliders
    drawSliderRow(m_window, m_font, "Musique",        m_settings.musicVolume, m_musicTrack);
    drawSliderRow(m_window, m_font, "Effets sonores", m_settings.sfxVolume,   m_sfxTrack);

    // Separateur
    sf::RectangleShape sep2({ m_panelW - 40.f, 1.f });
    sep2.setPosition({ m_panelX + 20.f, m_panelY + 210.f });
    sep2.setFillColor(sf::Color(180, 140, 60, 60));
    m_window.draw(sep2);

    // Plein ecran
    drawLabel(m_window, m_font, "Plein ecran",
              { labelX, m_fullscreenBox.position.y + 2.f });
    {
        sf::RectangleShape box({ 24.f, 24.f });
        box.setPosition(m_fullscreenBox.position);
        box.setFillColor(m_settings.fullscreen
            ? sf::Color(220, 175, 60) : sf::Color(40, 40, 50));
        box.setOutlineColor(sf::Color(180, 140, 60));
        box.setOutlineThickness(2.f);
        m_window.draw(box);
        if (m_settings.fullscreen) {
            sf::Text tick(m_font, "v", 16u);
            tick.setFillColor(sf::Color(20, 20, 20));
            sf::FloatRect tb2 = tick.getLocalBounds();
            tick.setPosition({
                m_fullscreenBox.position.x + (24.f - tb2.size.x) / 2.f - tb2.position.x,
                m_fullscreenBox.position.y + (24.f - tb2.size.y) / 2.f - tb2.position.y
            });
            m_window.draw(tick);
        }
    }

    // Separateur
    sf::RectangleShape sep3({ m_panelW - 40.f, 1.f });
    sep3.setPosition({ m_panelX + 20.f, m_panelY + 270.f });
    sep3.setFillColor(sf::Color(180, 140, 60, 60));
    m_window.draw(sep3);

    // Langue
    drawLabel(m_window, m_font, "Langue",
              { labelX, m_langLeftBtn.position.y + 4.f });
    {
        float trackW = m_langRightBtn.position.x + 28.f - m_langLeftBtn.position.x;

        // Bouton gauche
        sf::RectangleShape lBtn({ 28.f, 28.f });
        lBtn.setPosition(m_langLeftBtn.position);
        lBtn.setFillColor(sf::Color(45, 45, 55));
        lBtn.setOutlineColor(sf::Color(180, 140, 60));
        lBtn.setOutlineThickness(1.f);
        m_window.draw(lBtn);
        sf::Text lt(m_font, "<", 18u);
        lt.setFillColor(sf::Color::White);
        lt.setPosition({ m_langLeftBtn.position.x + 6.f, m_langLeftBtn.position.y + 2.f });
        m_window.draw(lt);

        // Valeur
        const std::string& lang = GameSettings::LANGUAGES[m_settings.languageIdx];
        sf::Text lv(m_font, lang, 17u);
        lv.setFillColor(sf::Color(220, 220, 220));
        sf::FloatRect lvb = lv.getLocalBounds();
        lv.setPosition({
            m_langLeftBtn.position.x + 28.f + (trackW - 56.f - lvb.size.x) / 2.f - lvb.position.x,
            m_langLeftBtn.position.y + 4.f
        });
        m_window.draw(lv);

        // Bouton droit
        sf::RectangleShape rBtn({ 28.f, 28.f });
        rBtn.setPosition(m_langRightBtn.position);
        rBtn.setFillColor(sf::Color(45, 45, 55));
        rBtn.setOutlineColor(sf::Color(180, 140, 60));
        rBtn.setOutlineThickness(1.f);
        m_window.draw(rBtn);
        sf::Text rt(m_font, ">", 18u);
        rt.setFillColor(sf::Color::White);
        rt.setPosition({ m_langRightBtn.position.x + 6.f, m_langRightBtn.position.y + 2.f });
        m_window.draw(rt);
    }

    m_window.display();
}