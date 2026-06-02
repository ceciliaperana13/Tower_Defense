#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <string>

struct GameSettings {
    float musicVolume { 50.f };
    float sfxVolume   { 50.f };
    bool  fullscreen  { false };
    int   languageIdx { 0 };

    static const std::vector<std::string> LANGUAGES;
};

class SettingsMenu {
public:
    explicit SettingsMenu(sf::RenderWindow& window, GameSettings& settings);
    ~SettingsMenu() = default;

    void run();

private:
    void buildUI();
    void handleEvents();
    void update(sf::Vector2f mouse);
    void render();

    // UI helpers
    void drawSliderRow(const std::string& label, float value, sf::FloatRect track);
    void drawLabel    (const std::string& txt,   sf::Vector2f pos);

    // Actions — délèguent au SoundManager
    void onMusicSlider(float mx);
    void onSfxSlider  (float mx);
    void onFullscreenToggle();
    void onLanguageNext();
    void onLanguagePrev();

    sf::RenderWindow& m_window;
    GameSettings&     m_settings;

    sf::Font                m_font;
    std::optional<sf::Text> m_titleText;
    std::optional<sf::Text> m_closeLabel;

    sf::RectangleShape m_panel;
    sf::RectangleShape m_closeBtn;

    sf::FloatRect m_musicTrack;
    sf::FloatRect m_sfxTrack;
    sf::FloatRect m_fullscreenBox;
    sf::FloatRect m_langLeftBtn;
    sf::FloatRect m_langRightBtn;

    bool m_draggingMusic { false };
    bool m_draggingSfx   { false };
    bool m_running       { true  };

    float m_panelX {}, m_panelY {}, m_panelW {}, m_panelH {};
};