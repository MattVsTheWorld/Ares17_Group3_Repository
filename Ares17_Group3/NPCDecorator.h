#ifndef NPCDECORATOR
#define NPCDECORATOR
#include "NonPC.h"

// NPCDecorator implements AbstractNPC
class NPCDecorator : public AbstractNPC {
private:
	float timer = 0.7f;
public:
	NPCDecorator(AbstractNPC *n) { 
		npc = n; 
		if (n->getName() == "boss") { timer = 1.5f; }
	}
	~NPCDecorator() {
		//	delete this;
	}

	

	void moveNpc(vertex* v, float speed) { ; }
	queue<vertex*> findPath(AdjacencyList *adjList, int startId, int endId) { return npc->findPath(adjList, startId, endId); }
	bool update(Model* modelData, glm::mat4 view, glm::mat4 proj, float dt, Grid* _g, Player* player, GLuint shader, float speed) {

		if (npc->getHealth() <= 0) {

			npc->setState(DYING);
			if (timer > 0.0f) {
				
				npc->update(modelData, view, proj, dt, _g, player, shader, speed);
				timer -= dt;
				return true;
			}
			else {
				delete npc;
				delete this;
				return false;
			}
		}

		else {
			npc->update(modelData, view, proj, dt, _g, player, shader, speed);
			//render(modelData, view, proj);
			return true;
		}
	}

	void render(Model * modelData, glm::mat4 view, glm::mat4 proj, GLuint shader, Player *player) {
		//npc->render(modelData, view, proj, shader); //TODO: delete (unused)
	} // delegate render to npc data member

	void modifyHealth(double newHp) {
		npc->modifyHealth(newHp);
	}
	
	void setAttack(double atk) {
		npc->setAttack(atk);
	}

	double getAttack() { 
		return npc->getAttack(); }

	void setRange(double rng) {
		npc->setRange(rng);
	}

	double getRange() {
		return npc->getRange();
	}
	double getHealth() {
		return npc->getHealth();
	}

	void setAttackSpeed(double atkspd) {
		npc->setAttackSpeed(atkspd);
	}
	double getAttackSpeed() {
		return npc->getAttackSpeed();
	}

	void setState(npcState newState) { 
		npc->setState(newState);
	}
	npcState getState() { 
		return npc->getState();
	}


	string getName() {
		return npc->getName();
	}
	void setName(string name) {
		return npc->setName(name);
	}

protected:
	AbstractNPC * npc;
};

// All types of NPCDecorator
class Melee : public NPCDecorator {
public:
	Melee(AbstractNPC *n) : NPCDecorator(n) {
		modifyHealth(50);
		setAttack(getAttack() + 20);
		setAttackSpeed(getAttackSpeed() + 1.5);
		setName("enforcer");
	}
	void modifyHealth(double newHp) { this->npc->modifyHealth(newHp); }
	double getRange() { return NPCDecorator::getRange(); } //TODO: maybe return a fixed melee range instead?
	double getHealth() { return NPCDecorator::getHealth(); }
	~Melee() { /*cout << "Deleting decorator" << endl;*/ }
};

class Ranged : public NPCDecorator {
public:
	Ranged(AbstractNPC *n) : NPCDecorator(n) {
		modifyHealth(30);
		setAttack(getAttack() + 10);
		setRange(getRange() + 15);
		setAttackSpeed(getAttackSpeed() + 3.0);
		setName("assault");
	}
	void modifyHealth(double newHp) { this->npc->modifyHealth(newHp); }
	double getRange() { return NPCDecorator::getRange(); }
	double getHealth() { return NPCDecorator::getHealth(); }
	~Ranged() { /*cout << "Deleting decorator" << endl;*/ }
};

class Light : public NPCDecorator {
public:
	Light(AbstractNPC *n) : NPCDecorator(n) {
		modifyHealth(10);
		setAttack(getAttack() + 10);
		setAttackSpeed(getAttackSpeed() + 1.0);
		setName("swarmer");
	}
	void modifyHealth(double newHp) { this->npc->modifyHealth(newHp); }
	double getRange() { return NPCDecorator::getRange(); }
	double getHealth() { return NPCDecorator::getHealth();/* + 10; */ }
	~Light() { /*cout << "Deleting decorator" << endl;*/ }
};

class Medium : public NPCDecorator {
public:
	Medium(AbstractNPC *n) : NPCDecorator(n) {
		modifyHealth(40);
		setAttack(getAttack() + 10);
		setAttackSpeed(getAttackSpeed() + 1.5);
		// normal atk speed
	}
	void modifyHealth(double newHp) { this->npc->modifyHealth(newHp); }
	double getRange() { return NPCDecorator::getRange(); }
	double getHealth() { return NPCDecorator::getHealth(); }
	~Medium() { /*cout << "Deleting decorator" << endl; */ }
};

class Heavy : public NPCDecorator {
public:
	Heavy(AbstractNPC *n) : NPCDecorator(n) {
		modifyHealth(150);
		setAttack(getAttack() + 50);
		setRange(getRange() + 5);
		setAttackSpeed(getAttackSpeed() + 3.0);
		setName("boss");
	}
	void modifyHealth(double newHp) { this->npc->modifyHealth(newHp); }
	double getRange() { return NPCDecorator::getRange(); }
	double getHealth() { return NPCDecorator::getHealth(); }
	~Heavy() { /*cout << "Deleting decorator" << endl;*/ }
};
#endif 