#pragma once
#include "Tower.hpp"

// ─────────────────────────────────────────────
// BasicTower
// ─────────────────────────────────────────────
class BasicTower : public Tower {
private:
    float bulletSpeed;

public:
    BasicTower(int x, int y);

    void attack(Enemy* e) override;
    void draw(sf::RenderWindow& window) override;
};

// ─────────────────────────────────────────────
// FireTower
// ─────────────────────────────────────────────
class FireTower : public Tower {
private:
    float burnDuration;

public:
    FireTower(int x, int y);

    void attack(Enemy* e) override;
    void draw(sf::RenderWindow& window) override;
};

// ─────────────────────────────────────────────
// EarthTower
// ─────────────────────────────────────────────
class EarthTower : public Tower {
private:
    float aoeRadius;

public:
    EarthTower(int x, int y);

    void attack(Enemy* e) override;
    void draw(sf::RenderWindow& window) override;
};

// ─────────────────────────────────────────────
// ArcaneTower
// ─────────────────────────────────────────────
class ArcaneTower : public Tower {
private:
    float manaPool;

public:
    ArcaneTower(int x, int y);

    void attack(Enemy* e) override;
    void draw(sf::RenderWindow& window) override;
};

// ─────────────────────────────────────────────
// IceTower
// ─────────────────────────────────────────────
class IceTower : public Tower {
private:
    float slowFactor;

public:
    IceTower(int x, int y);

    void attack(Enemy* e) override;
    void draw(sf::RenderWindow& window) override;
};