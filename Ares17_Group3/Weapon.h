#pragma once
#ifndef WEAPON
#define WEAPON

enum wepType { PISTOL, NUKA, SCIFI };

class Weapon {
public:

	Weapon() {};
	Weapon(wepType _type, double _dmg) {
		name = _type;
		damage = _dmg;
	}

	wepType getName() { return name; }
	double getDamage() { return damage; }

private:
	wepType name;
	double damage;
};

#endif