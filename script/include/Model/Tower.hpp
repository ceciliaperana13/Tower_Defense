#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

class Tower {
public:
    // Constructeur direct
    Tower(int id,
		int damage,
		int range,
		int fireRate,
		int nbTarget,
        const std::string& sprite1Path,
        const std::string& sprite2Path);

    // Constructeur depuis un fichier JSON
    static Tower fromJson(const std::string& jsonPath, const std::string& type);

    void targeting();
    void attack();

    void render(sf::RenderWindow& window);

    void setPosition(float x, float y);

    int getId()                                      const {return id;}
    void setId(int newId)                            {id = newId;}
    int getDamage ()                                 const {return damage;}
    void setDamage (int newDamage)                   {damage = newDamage;}
    int getRange ()                                  const {return range;}
    void setRange (int newRange)                     {range = newRange;} 
    int getFireRate ()                               const {return fireRate;}
    void setFireRate (int newFireRate)               {fireRate = newFireRate;}
    int getNbTarget ()                               const {return nbTarget;}
    void setNbTarget (int newNbTarget)               {nbTarget = newNbTarget;}
    int getLevel ()                                  const {return level;}

private:
    int id;
    int damage;
    int range;
    int fireRate;
    int damagePerSecond;
    int slowness;
    int effectDuration;
    int aoeRadius;
    int nbTarget;
    int targets;
    int level;

    sf::Texture               texture1;
    sf::Texture               texture2;
    std::optional<sf::Sprite> sprite;
};