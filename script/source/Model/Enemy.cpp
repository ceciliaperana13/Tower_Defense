#include "Enemy.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// ─── Constructeur direct 
Enemy::Enemy(int id,
             int maxhp,
             float maxspeed,
             int reward,
             const std::string& sprite1Path,
             const std::string& sprite2Path)
    : id(id),
      maxhp(maxhp),
      hp(maxhp),
      maxspeed(maxspeed),
      speed(maxspeed),
      reward(reward)
{
    if (!texture1.loadFromFile(sprite1Path))
        std::cerr << "[Enemy] Introuvable : " << sprite1Path << "\n";
    if (!texture2.loadFromFile(sprite2Path))
        std::cerr << "[Enemy] Introuvable : " << sprite2Path << "\n";

    sprite.emplace(texture1);
    sprite->setScale(sf::Vector2f(2.f, 2.f));
}

// ─── fromJson 
Enemy Enemy::fromJson(const std::string& jsonPath, const std::string& type)
{
    std::ifstream file(jsonPath);
    if (!file.is_open())
        throw std::runtime_error("[Enemy] Impossible d'ouvrir : " + jsonPath);

    json data = json::parse(file);

    if (!data["enemytype"].contains(type))
        throw std::runtime_error("[Enemy] Type inconnu dans le JSON : " + type);

    const auto& e = data["enemytype"][type];

    return Enemy(
        e["id"].get<int>(),
        e["maxhp"].get<int>(),
        e["maxspeed"].get<float>(),
        e["reward"].get<int>(),
        e["paths"]["sprite1"].get<std::string>(),
        e["paths"]["sprite2"].get<std::string>()
    );
}

// ─── update 
void Enemy::update(float dt) {
    animTimer += dt;
    if (animTimer >= 0.3f) {
        animTimer = 0.f;
        useFirst  = !useFirst;
        sprite->setTexture(useFirst ? texture1 : texture2);
    }

    sprite->move(sf::Vector2f(speed * dt, 0.f));
}

// ─── render 
void Enemy::render(sf::RenderWindow& window) {
    if (sprite.has_value())
        window.draw(*sprite);
}

// ─── setPosition
void Enemy::setPosition(float x, float y) {
    if (sprite.has_value())
        sprite->setPosition(sf::Vector2f(x, y));
}