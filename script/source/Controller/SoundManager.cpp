#include "Controller/SoundManager.hpp"
#include <iostream>
#include <algorithm>

SoundManager::SoundManager() {
    m_music = std::make_unique<sf::Music>();
}

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

void SoundManager::playMusic(const std::string& id) {
    if (m_currentMusicId == id && m_music &&
        m_music->getStatus() == sf::SoundSource::Status::Playing)
        return;

    m_music->stop();

    const std::string path = "../assets/sound/music/Medieval.mp3";
    if (!m_music->openFromFile(path)) {
        std::cerr << "[SoundManager] Impossible de charger : " << path << "\n";
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

//  met en pause sans perdre la position dans le morceau
void SoundManager::pauseMusic() {
    if (m_music &&
        m_music->getStatus() == sf::SoundSource::Status::Playing)
        m_music->pause();
}

//  reprend là où on s'était arrêté
void SoundManager::resumeMusic() {
    if (m_music &&
        m_music->getStatus() == sf::SoundSource::Status::Paused)
        m_music->play();
}

void SoundManager::setMusicVolume(float v) {
    m_musicVolume = std::clamp(v, 0.f, 100.f);
    if (m_music && !m_muted)
        m_music->setVolume(m_musicVolume);
}

void SoundManager::loadSFX(const std::string& path) {
    auto entry = std::make_unique<SoundEntry>();
    if (!entry->buffer.loadFromFile(path)) {
        std::cerr << "[SoundManager] SFX introuvable : " << path << "\n";
        return;
    }
    entry->sound.setVolume(m_sfxVolume);
    m_sounds[path] = std::move(entry);
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
    m_sfxVolume = std::clamp(v, 0.f, 100.f);
    for (auto& [id, entry] : m_sounds)
        entry->sound.setVolume(m_muted ? 0.f : m_sfxVolume);
}

void SoundManager::toggleMute() {
    m_muted = !m_muted;
    if (m_music)
        m_music->setVolume(m_muted ? 0.f : m_musicVolume);
    for (auto& [id, entry] : m_sounds)
        entry->sound.setVolume(m_muted ? 0.f : m_sfxVolume);
}