#include "SoundManager.hpp"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <functional>

using json = nlohmann::json;

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

    SoundManager::getInstance().loadProjectileSFXConfig(
        "../assets/data/tower_values.json"
    ); 
}

void SoundManager::stopMusic() {
    if (m_music) m_music->stop();
    m_currentMusicId.clear();
}

void SoundManager::setMusicVolume(float v) {
    m_musicVolume = std::clamp(v, 0.f, 100.f);
    if (m_music && !m_muted)
        m_music->setVolume(m_musicVolume);
}

void SoundManager::loadSFX(const std::string& id, const std::string& path) {
    auto entry = std::make_unique<SoundEntry>();
    if (!entry->buffer.loadFromFile(path)) {
        std::cerr << "[SoundManager] SFX introuvable : " << path << "\n";
        return;
    }
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


void SoundManager::loadProjectileSFXConfig(const std::string& path) {
    if (m_projectileSfxLoaded)
        return;

    std::ifstream file(path);
    if (!file) {
        std::cerr << "[SoundManager] Impossible d'ouvrir le JSON : " << path << "\n";
        return;
    }

    try {
        file >> m_projectileSfxJson;
        m_projectileSfxLoaded = true;
    }
    catch (const std::exception& e) {
        std::cerr << "[SoundManager] JSON invalide : " << e.what() << "\n";
    }
}

std::optional<std::string> SoundManager::findProjectileSoundPath(int id) const {
    if (!m_projectileSfxLoaded)
        return std::nullopt;

    std::function<const json*(const json&)> searchById = [&](const json& node) -> const json* {
        if (!node.is_object())
            return nullptr;

        if (node.contains("id") && node["id"].is_number_integer() && node["id"].get<int>() == id)
            return &node;

        for (const auto& [key, value] : node.items()) {
            if (auto* found = searchById(value))
                return found;
        }

        return nullptr;
    };

    const json* elementNode = searchById(m_projectileSfxJson);
    if (!elementNode)
        return std::nullopt;

    if (!elementNode->contains("paths"))
        return std::nullopt;

    const auto& paths = (*elementNode)["paths"];
    if (!paths.contains("sound") || !paths["sound"].is_string())
        return std::nullopt;

    return paths["sound"].get<std::string>();
}

void SoundManager::playProjectileImpact(int id) {
    if (!m_projectileSfxLoaded)
        return;

    std::string soundKey = std::to_string(id);

    if (m_sounds.find(soundKey) == m_sounds.end()) {
        auto path = findProjectileSoundPath(id);
        if (!path) {
            std::cerr << "[SoundManager] Son introuvable pour l'id : " << id << "\n";
            return;
        }
        loadSFX(soundKey, *path);
    }

    playSFX(soundKey);
}