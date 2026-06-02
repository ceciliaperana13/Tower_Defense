#include "Tower.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>

class Tower {
	private:
		int id;
		int x;
		int y;
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
		string sprite1Path;
		string sprite2Path;

	public:
		int getId () {return id;};
		void setId (int newId) {id = newId;};

		int getX () {return x;};
		void setX (int newX) {x = newX;};

		int getY () {return y;};
		void setY (int newY) {y = newY;};

		int getDamage () {return damage;};
		void setDamage (int newDamage) {damage = newDamage;};

		int getRange () {return range;};
		void setRange (int newRange) {range = newRange;}; 

		int getFireRate () {return fireRate;};
		void setFireRate (int newFireRate) {fireRate = newFireRate;}; 

		int getDamagePerSecond () {return damagePerSecond;};
		void setDamagePerSecond (int newDamagePerSecond) {damagePerSecond = newDamagePerSecond;};

		int getSlowness () {return slowness;};
		void setSlowness (int newSlowness) {slowness = newSlowness;};

		int getEffectDuration () {return effectDuration;};
		void setEffectDuration (int newEffectDuration) {effectDuration = newEffectDuration;};

		int getAoeRadius () {return aoeRadius;};
		void setAoeRadius (int newAoeRadius) {aoeRadius = newAoeRadius;};

		int getNbTarget () {return nbTarget;};
		void setNbTarget (int newNbTarget) {nbTarget = newNbTarget;};

		int getLevel () {return level;};
		void ugrapeTower (int upgradeChoice) {
			if (level == 1) {
				level += 1;
                setId(upgradeChoice);
                setDamage(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/damage);
                setRange(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/range);
                setFireRate(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/firerate);
                setDamagePerSecond(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/damagepersecond);
                setSlowness(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/slowness);
                setEffectDuration(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/effectduration);
                setAoeRadius(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/aoeradius);
                setNbTarget(../../../assets/data/tower_values.json/“id” : getId()/attackdatas/nbtarget);
            }
        };

		void targeting () {
			do{
				if (enemy in targets out of radius) {
					targets -= enemy
				}
				if (enemy in radius) {
					targets += enemy
				}
			} while (targets.length < nbTarget);
		};

        void attack () {
            for (enemy in targets list) {
                enemy.takeDamages(damage)
                
                if (damagePerSecond > 0) {
                    ;
                }
                int slowness;
                int effectDuration;
                int aoeRadius;
            }
        };
	
}:
