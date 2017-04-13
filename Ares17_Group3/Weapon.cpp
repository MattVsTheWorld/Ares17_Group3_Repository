#include "Weapon.h"


Weapon::Weapon() {};
Weapon::Weapon(wepType _type, double _dmg) {
	name = _type;
	damage = _dmg;
}
wepType Weapon::getName() { return name; }
double Weapon::getDamage() { return damage; }
