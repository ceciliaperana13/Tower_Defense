#include "Tower.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <cmath>

using json = nlohmann::json;

Tower::Tower(int id, int damage, int range, int fireRate, int nbTarget,
             int damagePerSecond, int slowness, int effectDuration, int aoeRadius, int level,
			 const std::string& spriteBuildingPath,
			 const std::string& spriteProjectilePath,
             const std::vector<sf::Vector2f>& waypoints)
    : m_id(id), m_damage(damage), m_range(range),
      m_fireRate(fireRate), m_nbTarget(nbTarget),
	  m_damagePerSecond(damagePerSecond), m_slowness(slowness),
	  m_effectDuration(effectDuration), m_aoeRadius(aoeRadius),
	  m_level(level), m_waypoints(waypoints)
{
    if (!m_tex1.loadFromFile(spriteBuildingPath))
        std::cerr << "[Tower] Introuvable : " << spriteBuildingPath << "\n";
    if (!m_tex2.loadFromFile(spriteProjectilePath))
        std::cerr << "[Tower] Introuvable : " << spriteProjectilePath << "\n";

    m_sprite.emplace(m_tex1);
    m_sprite->setScale(sf::Vector2f(2.f, 2.f));

    if (!m_waypoints.empty())
        m_sprite->setPosition(m_waypoints[0]);
}

Tower Tower::fromJson(const std::string& jsonPath,
                      const std::string& type,
                      const std::vector<sf::Vector2f>& waypoints) {
    std::ifstream file(jsonPath);
    if (!file.is_open())
        throw std::runtime_error("[Tower] Impossible d'ouvrir : " + jsonPath);

    json data = json::parse(file);

    if (!data["lvl1"].contains(type))
        throw std::runtime_error("[Tower] Type inconnu : " + type);

    const auto& e = data["lvl1"][type];

    return Tower(
        e["id"].get<int>(),
        e["damage"].get<int>(),
        e["range"].get<int>(),
        e["firerate"].get<int>(),
        e["nbtarget"].get<int>(),
        e["damagepersecond"].get<int>(),
        e["slowness"].get<int>(),
        e["effectduration"].get<int>(),
        e["aoeradius"].get<int>(),
        e["paths"]["building"].get<std::string>(),
        e["paths"]["projectile"].get<std::string>(),
        waypoints
    );
}

void Tower::upgrade() {

}

void Tower::targeting() {
	
}

void Tower::attack() {
	
}


// void Tower::moveAlongPath(float dt) {
//     if (m_reached || m_waypointIdx >= (int)m_waypoints.size())
//         return;

//     sf::Vector2f pos    = m_sprite->getPosition();
//     sf::Vector2f target = m_waypoints[m_waypointIdx];
//     sf::Vector2f delta  = target - pos;

//     float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
//     float step = m_speed * dt;

//     if (step >= dist) {
//         // Reached this waypoint, advance to the next
//         m_sprite->setPosition(target);
//         ++m_waypointIdx;

//         if (m_waypointIdx >= (int)m_waypoints.size()) {
//             m_reached = true;  // reached the castle
//         } else {
//             // Continue with remaining movement this frame
//             float remaining = step - dist;
//             sf::Vector2f next = m_waypoints[m_waypointIdx];
//             sf::Vector2f nextDelta = next - target;
//             float nextDist = std::sqrt(nextDelta.x*nextDelta.x + nextDelta.y*nextDelta.y);
//             if (nextDist > 0.f) {
//                 float t = std::min(remaining / nextDist, 1.f);
//                 m_sprite->setPosition(target + nextDelta * t);
//             }
//         }
//     } else {
//         sf::Vector2f dir = delta / dist;
//         m_sprite->move(dir * step);
//     }
// }

// void Tower::update(float dt) {
//     if (m_reached) return;

//     // Animate sprite
//     m_animTimer += dt;
//     if (m_animTimer >= 0.3f) {
//         m_animTimer = 0.f;
//         m_useFirst  = !m_useFirst;
//         m_sprite->setTexture(m_useFirst ? m_tex1 : m_tex2);
//     }

//     moveAlongPath(dt);
// }

// void Tower::render(sf::RenderWindow& window) const {
//     if (!m_reached && m_sprite.has_value())
//         window.draw(*m_sprite);
// }


// class Tower {
// 	private:
// 		int id;
// 		int x;
// 		int y;
// 		int damage;
// 		int range;
// 		int fireRate;
// 		int damagePerSecond;
// 		int slowness;
// 		int effectDuration;
// 		int aoeRadius;
// 		int waypoints;
// 		int targets;
// 		int level;
// 		string sprite1Path;
// 		string sprite2Path;

// 	public:
// 		int getId () {return id;};
// 		void setId (int newId) {id = newId;};

// 		int getX () {return x;};
// 		void setX (int newX) {x = newX;};

// 		int getY () {return y;};
// 		void setY (int newY) {y = newY;};

// 		int getDamage () {return damage;};
// 		void setDamage (int newDamage) {damage = newDamage;};

// 		int getRange () {return range;};
// 		void setRange (int newRange) {range = newRange;}; 

// 		int getFireRate () {return fireRate;};
// 		void setFireRate (int newFireRate) {fireRate = newFireRate;}; 

// 		int getDamagePerSecond () {return damagePerSecond;};
// 		void setDamagePerSecond (int newDamagePerSecond) {damagePerSecond = newDamagePerSecond;};

// 		int getSlowness () {return slowness;};
// 		void setSlowness (int newSlowness) {slowness = newSlowness;};

// 		int getEffectDuration () {return effectDuration;};
// 		void setEffectDuration (int newEffectDuration) {effectDuration = newEffectDuration;};

// 		int getAoeRadius () {return aoeRadius;};
// 		void setAoeRadius (int newAoeRadius) {aoeRadius = newAoeRadius;};

// 		int getwaypoints () {return waypoints;};
// 		void setwaypoints (int newwaypoints) {waypoints = newwaypoints;};

// 		int getLevel () {return level;};
// 		void ugrapeTower (int upgradeChoice) {
// 			if (level == 1) {
// 				level += 1;
//                 setId(upgradeChoice);
//                 setDamage(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/damage);
//                 setRange(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/range);
//                 setFireRate(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/firerate);
//                 setDamagePerSecond(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/damagepersecond);
//                 setSlowness(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/slowness);
//                 setEffectDuration(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/effectduration);
//                 setAoeRadius(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/aoeradius);
//                 setwaypoints(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/waypoints);
//             }
//         };

// 		void targeting () {
// 			do{
// 				if (Tower in targets out of radius) {
// 					targets -= Tower
// 				}
// 				if (Tower in radius) {
// 					targets += Tower
// 				}
// 			} while (targets.length < waypoints);
// 		};

//         void attack () {
//             for (Tower in targets list) {
//                 Tower.takeDamages(damage)
                
//                 if (damagePerSecond > 0) {
//                     ;
//                 }
//                 int slowness;
//                 int effectDuration;
//                 int aoeRadius;
//             }
//         };
	
// }