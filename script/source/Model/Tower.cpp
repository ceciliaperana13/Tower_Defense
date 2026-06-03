#include "Tower.hpp"

Tower::Tower(int x, int y, float damage, float range, float fireRate)
    : x(x), y(y), damage(damage), range(range), fireRate(fireRate), level(1)
{}

void Tower::upgrade() {
    level++;
    damage *= 1.2f;
    range  *= 1.1f;
}
