#include "SoundManager.hpp"
#include <iostream>

SoundManager::SoundManager() {
    loadSFX("click",     "../assets/sound/sfx/click.ogg");
    loadSFX("hover",     "../assets/sound/sfx/hover.ogg");
    loadSFX("explosion", "../assets/sound/sfx/explosion.ogg");
}

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

// ─── Music 
void SoundManager::playMusic(const std::string& id) {
    if (m_currentMusicId == id && m_music &&
        m_music->getStatus() == sf::SoundSource::Status::Playing)
        return;

    m_music = std::make_unique<sf::Music>();
    std::string path = "../assets/sound/music/" + id + ".ogg";

    if (!m_music->openFromFile(path)) {
        std::cerr << "[SoundManager] Musique introuvable : " << path << "\n";
        return;
    }

    m_currentMusicId = id;
    m_music->setLooping(true);
    m_music->setVolume(m_muted ? 0.f : m_musicVolume);
    m_music->play();
}

void SoundManager::stopMusic() {
    if (m_music) m_music->stop();
    m_currentMusicId.clear();
}

void SoundManager::setMusicVolume(float v) {
    m_musicVolume = std::max(0.f, std::min(100.f, v));
    if (m_music && !m_muted)
        m_music->setVolume(m_musicVolume);
}

// ─── SFX 
void SoundManager::loadSFX(const std::string& id, const std::string& path) {
    auto entry = std::make_unique<SoundEntry>();

    if (!entry->buffer.loadFromFile(path)) {
        std::cerr << "[SoundManager] SFX introuvable : " << path << "\n";
        return;
    }

    // sound est déjà lié au buffer via initialisation dans le struct
    entry->sound.setVolume(m_sfxVolume);
    m_sounds[id] = std::move(entry);
}

void SoundManager::playSFX(const std::string& id) {
    auto it = m_sounds.find(id);
    if (it == m_sounds.end()) {
        std::cerr << "[SoundManager] SFX inconnu : " << id << "\n";
        return;
    }
    if (!m_muted)
        it->second->sound.play();
}

void SoundManager::setSFXVolume(float v) {
    m_sfxVolume = std::max(0.f, std::min(100.f, v));
    for (auto& [id, entry] : m_sounds)
        entry->sound.setVolume(m_muted ? 0.f : m_sfxVolume);
}

// ─── Mute 
void SoundManager::toggleMute() {
    m_muted = !m_muted;
    if (m_music)
        m_music->setVolume(m_muted ? 0.f : m_musicVolume);
    for (auto& [id, entry] : m_sounds)
        entry->sound.setVolume(m_muted ? 0.f : m_sfxVolume);
}