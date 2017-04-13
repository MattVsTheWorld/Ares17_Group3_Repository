#pragma once
#ifndef WEAPON
#define WEAPON

enum wepType { PISTOL, NUKA, SCIFI };

class Weapon {
public:
	Weapon();
	Weapon(wepType _type, double _dmg);
	wepType getName();
	double getDamage();

private:
	wepType name;
	double damage;
};

#endif