#pragma once
#ifndef WEAPON
#define WEAPON

enum wepType { PISTOL, NUKA, SCIFI };

// Simple weapon class, could be expanded a lot
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