#include "Towers.hpp"
#include <iostream>

// ─────────────────────────────────────────────
// BasicTower
// ─────────────────────────────────────────────
BasicTower::BasicTower(int x, int y)
    : Tower(x, y, 10.f, 150.f, 1.0f), bulletSpeed(300.f)
{}

void BasicTower::attack(Enemy* e) {
    // Fires a standard bullet toward the enemy
    std::cout << "[BasicTower] Firing bullet at enemy (speed=" << bulletSpeed << ")\n";
}

void BasicTower::draw(sf::RenderWindow& window) {
    sf::RectangleShape shape({32.f, 32.f});
    shape.setFillColor(sf::Color(100, 180, 255));
    shape.setPosition({static_cast<float>(x), static_cast<float>(y)});
    window.draw(shape);
}

// ─────────────────────────────────────────────
// FireTower
// ─────────────────────────────────────────────
FireTower::FireTower(int x, int y)
    : Tower(x, y, 20.f, 120.f, 0.8f), burnDuration(3.0f)
{}

void FireTower::attack(Enemy* e) {
    // Applies burn damage over time
    std::cout << "[FireTower] Burning enemy for " << burnDuration << "s\n";
}

void FireTower::draw(sf::RenderWindow& window) {
    sf::RectangleShape shape({32.f, 32.f});
    shape.setFillColor(sf::Color(255, 100, 30));
    shape.setPosition({static_cast<float>(x), static_cast<float>(y)});
    window.draw(shape);
}

// ─────────────────────────────────────────────
// EarthTower
// ─────────────────────────────────────────────
EarthTower::EarthTower(int x, int y)
    : Tower(x, y, 25.f, 100.f, 0.6f), aoeRadius(60.f)
{}

void EarthTower::attack(Enemy* e) {
    // Deals AoE damage around the target
    std::cout << "[EarthTower] AoE attack (radius=" << aoeRadius << ")\n";
}

void EarthTower::draw(sf::RenderWindow& window) {
    sf::RectangleShape shape({32.f, 32.f});
    shape.setFillColor(sf::Color(80, 200, 80));
    shape.setPosition({static_cast<float>(x), static_cast<float>(y)});
    window.draw(shape);
}

// ─────────────────────────────────────────────
// ArcaneTower
// ─────────────────────────────────────────────
ArcaneTower::ArcaneTower(int x, int y)
    : Tower(x, y, 30.f, 180.f, 0.7f), manaPool(100.f)
{}

void ArcaneTower::attack(Enemy* e) {
    // Consumes mana to deal arcane damage
    if (manaPool > 0) {
        manaPool -= 10.f;
        std::cout << "[ArcaneTower] Arcane blast! Mana left: " << manaPool << "\n";
    } else {
        std::cout << "[ArcaneTower] Out of mana!\n";
    }
}

void ArcaneTower::draw(sf::RenderWindow& window) {
    sf::RectangleShape shape({32.f, 32.f});
    shape.setFillColor(sf::Color(220, 160, 220));
    shape.setPosition({static_cast<float>(x), static_cast<float>(y)});
    window.draw(shape);
}

// ─────────────────────────────────────────────
// IceTower
// ─────────────────────────────────────────────
IceTower::IceTower(int x, int y)
    : Tower(x, y, 8.f, 140.f, 1.2f), slowFactor(0.5f)
{}

void IceTower::attack(Enemy* e) {
    // Slows enemy movement
    std::cout << "[IceTower] Slowing enemy by " << (1.f - slowFactor) * 100.f << "%\n";
}

void IceTower::draw(sf::RenderWindow& window) {
    sf::RectangleShape shape({32.f, 32.f});
    shape.setFillColor(sf::Color(180, 230, 255));
    shape.setPosition({static_cast<float>(x), static_cast<float>(y)});
    window.draw(shape);
}