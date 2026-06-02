#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

class Enemy {
public:
    // Constructeur direct
    Enemy(int id,
          int maxhp,
          float maxspeed,
          int reward,
          const std::string& sprite1Path,
          const std::string& sprite2Path);

    // Constructeur depuis un fichier JSON
    static Enemy fromJson(const std::string& jsonPath, const std::string& type);

    void update(float dt);
    void render(sf::RenderWindow& window);

    void setPosition(float x, float y);
    bool isDead() const { return hp <= 0; }

    int  getId()     const { return id; }
    int  getHp()     const { return hp; }
    int  getReward() const { return reward; }

private:
    int   id;
    int   maxhp;
    int   hp;
    float maxspeed;
    float speed;
    int   reward;

    sf::Texture               texture1;
    sf::Texture               texture2;
    std::optional<sf::Sprite> sprite;

    float animTimer   { 0.f };
    bool  useFirst    { true };
};