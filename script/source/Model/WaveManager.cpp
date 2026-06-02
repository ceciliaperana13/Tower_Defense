#include "WaveManager.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

using json = nlohmann::json;

// ─── Constructeur ─────────────────────────────────────────────────────────────
WaveManager::WaveManager(const std::string& wavesJson,
                         const std::string& enemiesJson)
    : m_enemiesJson(enemiesJson)
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

// ─── startNextWave ────────────────────────────────────────────────────────────
void WaveManager::startNextWave() {
    if (allWavesDone()) return;

    ++m_currentWave;
    m_currentGroup    = 0;
    m_spawnedInGroup  = 0;
    m_spawnTimer      = 0.f;
    m_state           = State::Spawning;

    std::cout << "[WaveManager] Vague " << getCurrentWaveId()
              << " | speedx" << m_waves[m_currentWave].speedMultiplier << "\n";
}

// ─── update ───────────────────────────────────────────────────────────────────
void WaveManager::update(float dt) {
    // Met à jour les ennemis actifs, retire les morts
    for (auto& e : m_activeEnemies)
        e->update(dt);

    m_activeEnemies.erase(
        std::remove_if(m_activeEnemies.begin(), m_activeEnemies.end(),
            [](const std::unique_ptr<Enemy>& e) { return e->isDead(); }),
        m_activeEnemies.end()
    );

    if (m_state == State::Idle || m_state == State::WaveComplete)
        return;

    const Wave& wave = m_waves[m_currentWave];

    // ── Pause entre groupes ──────────────────────────────────────────────────
    if (m_state == State::PauseBetweenGroups) {
        m_pauseTimer -= dt;
        if (m_pauseTimer <= 0.f) {
            m_state          = State::Spawning;
            m_spawnedInGroup = 0;
            m_spawnTimer     = 0.f;
        }
        return;
    }

    // ── Spawn du groupe courant ───────────────────────────────────────────────
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

// ─── spawnNext ────────────────────────────────────────────────────────────────
void WaveManager::spawnNext() {
    const Wave&        wave  = m_waves[m_currentWave];
    const EnemyGroup&  group = wave.groups[m_currentGroup];

    try {
        // Charge les stats depuis enemy_values.json
        auto enemy = std::make_unique<Enemy>(
            Enemy::fromJson(m_enemiesJson, group.type)
        );

        // Applique le multiplicateur de vitesse de la vague
        enemy->applySpeedMultiplier(wave.speedMultiplier);

        // TODO: position de spawn selon le chemin de la map
        enemy->setPosition(0.f, 300.f);

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

// ─── isWaveComplete ───────────────────────────────────────────────────────────
bool WaveManager::isWaveComplete() const {
    return m_state == State::WaveComplete && m_activeEnemies.empty();
}
