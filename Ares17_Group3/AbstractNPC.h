#ifndef ABSTRACTNPC
#define ABSTRACTNPC
#include "btShapeManager.h"
#include <iostream>

#include <iostream>

using std::cout;
using std::endl;
using std::cin;


// Abstract base class: no implementation
class AbstractNPC {
public:
//	virtual btRigidBody* addBoundingCapsule() = 0;
	virtual void render(Model * modelData, glm::mat4 view, glm::mat4 proj) = 0;
	virtual void setHealth(double newHp) = 0;
//	virtual bool findCollision() = 0;
	virtual double getRange() = 0;
	virtual double getHealth() = 0;
	virtual ~AbstractNPC() {}
};


//	/// !!!
//	AbstractNPC *monsterArray[10];
//	monsterArray[0] = new Elite(new NPC("Orc", 10, 40));
//	monsterArray[1] = new Captain(new NPC("Elf", 50, 60));
//
//
//	for (int i = 0;i<2;i++)
//	{
//		cout << "0 : ";monsterArray[i]->render();
//		cout << " Health : ";cout << monsterArray[i]->getHealth(); cout << endl;
//		cout << " Mana : ";cout << monsterArray[i]->getMana(); cout << endl;
//		delete monsterArray[i];
//
//	}
//	/// !!!


#endif