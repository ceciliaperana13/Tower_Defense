#pragma once
#include "Enemy.hpp"
#include "json.hpp"
#include <vector>
#include <string>
#include <memory>

struct EnemyGroup {
    std::string type;
    int         count;
    float       spawnInterval;
};

struct Wave {
    int                     id;
    float                   speedMultiplier;
    std::vector<EnemyGroup> groups;
    float                   pauseBetweenGroups;
};

class WaveManager {
public:
    WaveManager(const std::string& wavesJson,
                const std::string& enemiesJson);

    void update(float dt);
    void startNextWave();

    std::vector<std::unique_ptr<Enemy>>& getActiveEnemies() { return m_activeEnemies; }

    bool isWaveComplete()   const;
    bool allWavesDone()     const { return m_currentWave >= (int)m_waves.size(); }
    int  getCurrentWaveId() const { return m_currentWave + 1; }
    int  getTotalWaves()    const { return (int)m_waves.size(); }

private:
    void spawnNext();

    std::string               m_enemiesJson;
    std::vector<Wave>         m_waves;
    std::vector<std::unique_ptr<Enemy>> m_activeEnemies;

    int   m_currentWave    { -1 };
    int   m_currentGroup   {  0 };
    int   m_spawnedInGroup {  0 };
    float m_spawnTimer     { 0.f };
    float m_pauseTimer     { 0.f };

    enum class State { Idle, Spawning, PauseBetweenGroups, WaveComplete };
    State m_state { State::Idle };
};