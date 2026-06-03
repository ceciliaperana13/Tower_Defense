#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include "Enemy.hpp"

class Tower {
public:
    // Constructeur direct
    Tower(int id, int damage, int range, int fireRate, int nbTarget,
        int damagePerSecond, int slowness, int effectDuration, int aoeRadius, int level,
        const std::string& spriteBuildingPath,
        const std::string& spriteProjectilePath,
        const std::vector<sf::Vector2f>& waypoints);

    // Constructeur depuis un fichier JSON
    static Tower fromJson(const std::string& jsonPath, const std::string& type, const std::vector<sf::Vector2f>& waypoints);

    void upgrade();
    void targeting();
    virtual void attack();

    void render(sf::RenderWindow& window);

    void setPosition(float x, float y);

    int  getId()                                      const {return m_id;}
    void setId(int newId)                            {m_id = newId;}
    int  getDamage ()                                 const {return m_damage;}
    void setDamage (int newDamage)                   {m_damage = newDamage;}
    int  getRange ()                                  const {return m_range;}
    void setRange (int newRange)                     {m_range = newRange;} 
    int  getFireRate ()                               const {return m_fireRate;}
    void setFireRate (int newFireRate)               {m_fireRate = newFireRate;}
    int  getNbTarget ()                               const {return m_nbTarget;}
    void setNbTarget (int newNbTarget)               {m_nbTarget = newNbTarget;}
    int  getDamagePerSecond ()                        const {return m_damagePerSecond;}
    void setDamagePerSecond (int newDamagePerSecond) {m_damagePerSecond = newDamagePerSecond;}
    int  getSlowness ()                               const {return m_slowness;}
    void setSlowness (int newSlowness)               {m_slowness = newSlowness;}
    int  getEffectDuration ()                         const {return m_effectDuration;}
    void setEffectDuration (int newEffectDuration)   {m_effectDuration = newEffectDuration;}
    int  getAoeRadius ()                              const {return m_aoeRadius;}
    void setAoeRadius (int newAoeRadius)             {m_aoeRadius = newAoeRadius;}
    int  getLevel ()                                  const {return m_level;}

private:
    int m_id;
    int m_damage;
    int m_range;
    int m_fireRate;
    int m_nbTarget;
    int m_damagePerSecond;
    int m_slowness;
    int m_effectDuration;
    int m_aoeRadius;
    int m_level;
    Enemy m_targets;

    sf::Texture               m_tex1;
    sf::Texture               m_tex2;
    std::optional<sf::Sprite> m_sprite;

    std::vector<sf::Vector2f> m_waypoints;
};