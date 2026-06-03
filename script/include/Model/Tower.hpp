#pragma once
#include <SFML/Graphics.hpp>

class Enemy; // forward declaration

class Tower {
protected:
    int x;
    int y;
    float damage;
    float range;
    float fireRate;
    int level;

public:
    Tower(int x, int y, float damage, float range, float fireRate);
    virtual ~Tower() = default;

    virtual void attack(Enemy* e) = 0;
    virtual void upgrade();

    int getX() const { return x; }
    int getY() const { return y; }
    float getDamage() const { return damage; }
    float getRange() const { return range; }
    float getFireRate() const { return fireRate; }
    int getLevel() const { return level; }

    virtual void draw(sf::RenderWindow& window) = 0;
};