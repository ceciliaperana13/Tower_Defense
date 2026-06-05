#include "Model/WaveManager.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

using json = nlohmann::json;

// ─── Constructeur
WaveManager::WaveManager(const std::string& wavesJson,
                         const std::string& enemiesJson,
                         const std::vector<sf::Vector2f>& waypoints)
    : m_enemiesJson(enemiesJson)
    , m_waypoints(waypoints)
{
    std::ifstream file(wavesJson);
    if (!file.is_open())
        throw std::runtime_error("[WaveManager] Impossible d'ouvrir : " + wavesJson);

    json data = json::parse(file);

    for (const auto& w : data["waves"]) {
        Wave wave;
        wave.id                 = w["id"].get<int>();
        wave.speedMultiplier    = w["speedMultiplier"].get<float>();
        wave.pauseBetweenGroups = w["pauseBetweenGroups"].get<float>();

        for (const auto& g : w["groups"]) {
            EnemyGroup group;
            group.type          = g["type"].get<std::string>();
            group.count         = g["count"].get<int>();
            group.spawnInterval = g["spawnInterval"].get<float>();
            wave.groups.push_back(group);
        }

        m_waves.push_back(std::move(wave));
    }

    std::cout << "[WaveManager] " << m_waves.size() << " vagues chargées.\n";
}

// ─── startNextWave
void WaveManager::startNextWave() {
    if (allWavesDone()) return;

    ++m_currentWave;
    m_currentGroup   = 0;
    m_spawnedInGroup = 0;
    m_spawnTimer     = 0.f;
    m_state          = State::Spawning;

    std::cout << "[WaveManager] Vague " << getCurrentWaveId()
              << " | speedx" << m_waves[m_currentWave].speedMultiplier << "\n";
}

// ─── update
void WaveManager::update(float dt) {
    // Met à jour tous les ennemis actifs
    for (auto& e : m_activeEnemies)
        e->update(dt);

    // Compte les ennemis tués ET les ennemis arrivés, avant suppression
    for (auto& e : m_activeEnemies) {
        if (e->hasReached())        ++m_totalReached;  // atteint le château
        if (e->isDead() && !e->hasReached()) ++m_totalKills;  // tué par une tour
    }

    // Supprime les ennemis morts ou arrivés
    m_activeEnemies.erase(
        std::remove_if(m_activeEnemies.begin(), m_activeEnemies.end(),
            [](const std::unique_ptr<Enemy>& e) {
                return e->isDead() || e->hasReached();
            }),
        m_activeEnemies.end()
    );

    if (m_state == State::Idle)
        return;

    // Vague terminée : attend puis lance la suivante automatiquement
    if (m_state == State::WaveComplete && m_activeEnemies.empty()) {
        if (!allWavesDone()) {
            m_state     = State::WaitingNextWave;
            m_waveTimer = INTER_WAVE_DELAY;
        }
        return;
    }

    if (m_state == State::WaitingNextWave) {
        m_waveTimer -= dt;
        if (m_waveTimer <= 0.f)
            startNextWave();
        return;
    }

    const Wave& wave = m_waves[m_currentWave];

    if (m_state == State::PauseBetweenGroups) {
        m_pauseTimer -= dt;
        if (m_pauseTimer <= 0.f) {
            m_state          = State::Spawning;
            m_spawnedInGroup = 0;
            m_spawnTimer     = 0.f;
        }
        return;
    }

    if (m_state == State::Spawning) {
        const EnemyGroup& group = wave.groups[m_currentGroup];

        m_spawnTimer -= dt;
        if (m_spawnTimer <= 0.f && m_spawnedInGroup < group.count) {
            spawnNext();
            m_spawnTimer = group.spawnInterval;
        }

        if (m_spawnedInGroup >= group.count) {
            ++m_currentGroup;

            if (m_currentGroup < (int)wave.groups.size()) {
                m_state      = State::PauseBetweenGroups;
                m_pauseTimer = wave.pauseBetweenGroups;
            } else {
                m_state = State::WaveComplete;
                std::cout << "[WaveManager] Spawn terminé vague "
                          << getCurrentWaveId() << ".\n";
            }
        }
    }
}

// ─── spawnNext
void WaveManager::spawnNext() {
    const Wave&       wave  = m_waves[m_currentWave];
    const EnemyGroup& group = wave.groups[m_currentGroup];

    try {
        auto enemy = std::make_unique<Enemy>(
            Enemy::fromJson(m_enemiesJson, group.type, m_waypoints)
        );
        enemy->applySpeedMultiplier(wave.speedMultiplier);

        m_activeEnemies.push_back(std::move(enemy));
        ++m_spawnedInGroup;

        std::cout << "[WaveManager] Spawn " << group.type
                  << " (" << m_spawnedInGroup << "/" << group.count
                  << ") speed x" << wave.speedMultiplier << "\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "[WaveManager] Erreur spawn : " << ex.what() << "\n";
    }
}

// ─── isWaveComplete
bool WaveManager::isWaveComplete() const {
    return (m_state == State::WaveComplete || m_state == State::WaitingNextWave)
           && m_activeEnemies.empty();
}