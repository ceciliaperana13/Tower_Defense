#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>

struct SoundEntry {
    sf::SoundBuffer buffer;
    sf::Sound       sound { buffer };
};

class SoundManager {
public:
    static SoundManager& getInstance();

    void playMusic(const std::string& id);
    void stopMusic();
    void pauseMusic();   // ← nouveau
    void resumeMusic();  // ← nouveau
    void setMusicVolume(float v);

    void playSFX(const std::string& id);
    void setSFXVolume(float v);

    void toggleMute();
    bool  isMuted()        const { return m_muted; }
    float getMusicVolume() const { return m_musicVolume; }
    float getSFXVolume()   const { return m_sfxVolume; }

private:
    SoundManager();
    ~SoundManager() = default;
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    void loadSFX(const std::string& id, const std::string& path);

    float m_musicVolume { 50.f };
    float m_sfxVolume   { 50.f };
    bool  m_muted       { false };

    std::string                m_currentMusicId;
    std::unique_ptr<sf::Music> m_music;

    std::unordered_map<std::string, std::unique_ptr<SoundEntry>> m_sounds;
};