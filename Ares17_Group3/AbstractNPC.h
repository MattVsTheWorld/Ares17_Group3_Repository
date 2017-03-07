#ifndef ABSTRACTNPC
#define ABSTRACTNPC
#include "btShapeManager.h"
#include <iostream>

#include <iostream>
// Instead of importing the entire std namespace, we can be
// specific about which std library classes and objects are to be included:
using std::cout;
using std::endl;
using std::cin;


// Abstract base class: no implementation
class AbstractNPC {
public:
	virtual btRigidBody* addBoundingCapsule() = 0;
	virtual void render() = 0;
	virtual bool findCollision() = 0;
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