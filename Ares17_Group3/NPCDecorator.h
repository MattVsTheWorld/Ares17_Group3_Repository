#ifndef NPCDECORATOR
#define NPCDECORATOR
#include "NonPC.h"


// NPCDecorator implements AbstractNPC
class NPCDecorator : public AbstractNPC {
private:
	AbstractNPC * npc;
public:
	NPCDecorator(AbstractNPC *n) { npc = n; }
	void render() { npc->render(); } // delegate render to npc data member
	double getRange() { return npc->getRange(); }
	double getHealth() { return npc->getHealth(); }
};

// All types of NPCDecorator
class Melee : public NPCDecorator {
public:
	Melee(AbstractNPC *n) : NPCDecorator(n) { }
	void render() {
		// render special features 
		// TODO:might change material?
		// apply different texture?
		NPCDecorator::render(); // delegate to base class
	}
	double getRange() { return NPCDecorator::getRange(); } //TODO: maybe return a fixed melee range instead?
	double getHealth() { return NPCDecorator::getHealth() + 50; }
	~Melee() { /*cout << "Deleting decorator" << endl;*/ }
};

class Ranged : public NPCDecorator {
public:
	Ranged(AbstractNPC *n) : NPCDecorator(n) { }
	void render() {
		// render special features 
		// TODO:might change material?
		// apply different texture?
		NPCDecorator::render(); // delegate to base class
	}
	double getRange() { return NPCDecorator::getRange() + 30; }
	double getHealth() { return NPCDecorator::getHealth() - 20; }
	~Ranged() { /*cout << "Deleting decorator" << endl;*/ }
};

class Light : public NPCDecorator {
public:
	Light(AbstractNPC *n) : NPCDecorator(n) { }
	void render() {
		// render special features 
		// TODO:might change material?
		// apply different texture?
		NPCDecorator::render(); // delegate to base class
	}
	double getRange() { return NPCDecorator::getRange(); }
	double getHealth() { return NPCDecorator::getHealth() + 10; }
	~Light() { /*cout << "Deleting decorator" << endl;*/ }
};

class Medium : public NPCDecorator {
public:
	Medium(AbstractNPC *n) : NPCDecorator(n) { }
	void render() {
		// render special features 
		// TODO:might change material?
		// apply different texture?
		NPCDecorator::render(); // delegate to base class
	}
	double getRange() { return NPCDecorator::getRange(); }
	double getHealth() { return NPCDecorator::getHealth() + 40; }
	~Medium() { /*cout << "Deleting decorator" << endl; */}
};

class Heavy : public NPCDecorator {
public:
	Heavy(AbstractNPC *n) : NPCDecorator(n) { }
	void render() {
		// render special features 
		// TODO:might change material?
		// apply different texture?
		NPCDecorator::render(); // delegate to base class
	}
	double getRange() { return NPCDecorator::getRange() + 5; }
	double getHealth() { return NPCDecorator::getHealth() + 200; }
	~Heavy() { /*cout << "Deleting decorator" << endl;*/ }
};
#endif 