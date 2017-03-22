#ifndef NPCDECORATOR
#define NPCDECORATOR
#include "NonPC.h"

// NPCDecorator implements AbstractNPC
class NPCDecorator : public AbstractNPC {
private:

public:
	NPCDecorator(AbstractNPC *n) { npc = n; }
	void render(Model * modelData, glm::mat4 view, glm::mat4 proj) { npc->render(modelData, view, proj); } // delegate render to npc data member
	void setHealth(double newHp) { npc->setHealth(newHp); }
	double getRange() { return npc->getRange(); }
	double getHealth() { return npc->getHealth(); }
protected:
	AbstractNPC * npc;
};

// All types of NPCDecorator
class Melee : public NPCDecorator {
public:
	Melee(AbstractNPC *n) : NPCDecorator(n) {
		setHealth(50);
	}
	//void render() {
	//	// render special features 
	//	// TODO:might change material?
	//	// apply different texture?
	//	Model * modelData;
	//	NPCDecorator::render(modelData); // delegate to base class
	//}
	void setHealth(double newHp) { this->npc->setHealth(newHp); }
	double getRange() { return NPCDecorator::getRange(); } //TODO: maybe return a fixed melee range instead?
	double getHealth() { return NPCDecorator::getHealth(); }
	~Melee() { /*cout << "Deleting decorator" << endl;*/ }
};

class Ranged : public NPCDecorator {
public:
	Ranged(AbstractNPC *n) : NPCDecorator(n) { 
		setHealth(30);
	}
	//void render() {
	//	// render special features 
	//	// TODO:might change material?
	//	// apply different texture?
	//	Model * modelData;
	//	NPCDecorator::render(modelData); // delegate to base class
	//}
	void setHealth(double newHp) { this->npc->setHealth(newHp); }
	double getRange() { return NPCDecorator::getRange(); }
	double getHealth() { return NPCDecorator::getHealth(); }
	~Ranged() { /*cout << "Deleting decorator" << endl;*/ }
};

class Light : public NPCDecorator {
public:
	Light(AbstractNPC *n) : NPCDecorator(n) { 
		setHealth(10);
	}
	//void render() {
	//	// render special features 
	//	// TODO:might change material?
	//	// apply different texture?
	//	Model * modelData;
	//	NPCDecorator::render(modelData); // delegate to base class
	//}
	void setHealth(double newHp) { this->npc->setHealth(newHp); }
	double getRange() { return NPCDecorator::getRange(); }
	double getHealth() { return NPCDecorator::getHealth();/* + 10; */ }
	~Light() { /*cout << "Deleting decorator" << endl;*/ }
};

class Medium : public NPCDecorator {
public:
	Medium(AbstractNPC *n) : NPCDecorator(n) {
		setHealth(40);
	}
	//void render() {
	//	// render special features 
	//	// TODO:might change material?
	//	// apply different texture?
	//	Model * modelData;
	//	NPCDecorator::render(modelData); // delegate to base class
	//}
	void setHealth(double newHp) { this->npc->setHealth(newHp); }
	double getRange() { return NPCDecorator::getRange(); }
	double getHealth() { return NPCDecorator::getHealth(); }
	~Medium() { /*cout << "Deleting decorator" << endl; */}
};

class Heavy : public NPCDecorator {
public:
	Heavy(AbstractNPC *n) : NPCDecorator(n) {
		setHealth(200);
	}
	//void render() {
	//	// render special features 
	//	// TODO:might change material?
	//	// apply different texture?
	//	Model * modelData;
	//	NPCDecorator::render(modelData); // delegate to base class
	//}
	void setHealth(double newHp) { this->npc->setHealth(newHp); }
	double getRange() { return NPCDecorator::getRange(); }
	double getHealth() { return NPCDecorator::getHealth(); }
	~Heavy() { /*cout << "Deleting decorator" << endl;*/ }
};
#endif 