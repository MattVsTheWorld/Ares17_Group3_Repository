#include "NPCDecorator.h"

NPCDecorator::NPCDecorator(AbstractNPC *n) {
	npc = n;
	if (n->getName() == "boss") { timer = 1.5f; }
}
NPCDecorator::~NPCDecorator() { }

void NPCDecorator::moveNpc(vertex* v, float speed) { ; }
queue<vertex*> NPCDecorator::findPath(AdjacencyList *adjList, int startId, int endId) { return npc->findPath(adjList, startId, endId); }
bool NPCDecorator::update(Model* modelData, glm::mat4 view, glm::mat4 proj, float dt, Grid* _g, Player* player, GLuint shader, float speed) {

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

void NPCDecorator::render(Model * modelData, glm::mat4 view, glm::mat4 proj, GLuint shader, Player *player) {
	//npc->render(modelData, view, proj, shader); //TODO: delete (unused)
} // delegate render to npc data member

void NPCDecorator::modifyHealth(double newHp) {
	npc->modifyHealth(newHp);
}

void NPCDecorator::setAttack(double atk) {
	npc->setAttack(atk);
}

double NPCDecorator::getAttack() {
	return npc->getAttack();
}

void NPCDecorator::setRange(double rng) {
	npc->setRange(rng);
}

double NPCDecorator::getRange() {
	return npc->getRange();
}
double NPCDecorator::getHealth() {
	return npc->getHealth();
}

void NPCDecorator::setAttackSpeed(double atkspd) {
	npc->setAttackSpeed(atkspd);
}
double NPCDecorator::getAttackSpeed() {
	return npc->getAttackSpeed();
}

void NPCDecorator::setState(npcState newState) {
	npc->setState(newState);
}
npcState NPCDecorator::getState() {
	return npc->getState();
}


string NPCDecorator::getName() {
	return npc->getName();
}
void NPCDecorator::setName(string name) {
	return npc->setName(name);
}

// All types of NPCDecorator

Melee::Melee(AbstractNPC *n) : NPCDecorator(n) {
	modifyHealth(50);
	setAttack(getAttack() + 20);
	setAttackSpeed(getAttackSpeed() + 1.5);
	setName("enforcer");
}
void Melee::modifyHealth(double newHp) { this->npc->modifyHealth(newHp); }
double Melee::getRange() { return NPCDecorator::getRange(); } //TODO: maybe return a fixed melee range instead?
double Melee::getHealth() { return NPCDecorator::getHealth(); }
Melee::~Melee() { /*cout << "Deleting decorator" << endl;*/ }


Ranged::Ranged(AbstractNPC *n) : NPCDecorator(n) {
	modifyHealth(30);
	setAttack(getAttack() + 10);
	setRange(getRange() + 15);
	setAttackSpeed(getAttackSpeed() + 3.0);
	setName("assault");
}
void Ranged::modifyHealth(double newHp) { this->npc->modifyHealth(newHp); }
double Ranged::getRange() { return NPCDecorator::getRange(); }
double Ranged::getHealth() { return NPCDecorator::getHealth(); }
Ranged::~Ranged() { /*cout << "Deleting decorator" << endl;*/ }



Light::Light(AbstractNPC *n) : NPCDecorator(n) {
	modifyHealth(10);
	setAttack(getAttack() + 10);
	setAttackSpeed(getAttackSpeed() + 1.0);
	setName("swarmer");
}
void Light::modifyHealth(double newHp) { this->npc->modifyHealth(newHp); }
double Light::getRange() { return NPCDecorator::getRange(); }
double Light::getHealth() { return NPCDecorator::getHealth();/* + 10; */ }
Light::~Light() { /*cout << "Deleting decorator" << endl;*/ }


Medium::Medium(AbstractNPC *n) : NPCDecorator(n) {
	modifyHealth(40);
	setAttack(getAttack() + 10);
	setAttackSpeed(getAttackSpeed() + 1.5);
	// normal atk speed
}
void Medium::modifyHealth(double newHp) { this->npc->modifyHealth(newHp); }
double Medium::getRange() { return NPCDecorator::getRange(); }
double Medium::getHealth() { return NPCDecorator::getHealth(); }
Medium::~Medium() { /*cout << "Deleting decorator" << endl; */ }



Heavy::Heavy(AbstractNPC *n) : NPCDecorator(n) {
	modifyHealth(150);
	setAttack(getAttack() + 50);
	setRange(getRange() + 5);
	setAttackSpeed(getAttackSpeed() + 3.0);
	setName("boss");
}
void Heavy::modifyHealth(double newHp) { this->npc->modifyHealth(newHp); }
double Heavy::getRange() { return NPCDecorator::getRange(); }
double Heavy::getHealth() { return NPCDecorator::getHealth(); }
Heavy::~Heavy() { /*cout << "Deleting decorator" << endl;*/ }
