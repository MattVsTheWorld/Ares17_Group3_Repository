#ifndef NONPC
#define NONPC
#include "AbstractNPC.h"

#define PI 3.14159265359f
#define REFRESHRATE 1.0f
#define DEFAULT_LOS 50.0f
#define DEG_TO_RADIAN 0.017453293f

// NPC implements AbstractNPC - all methods defined inline
class NonPC : public AbstractNPC {
private:
	bool moved;
	float angle;
	bool findCollision(btPairCachingGhostObject* ghostObject);
	static void toEulerianAngle(const btQuaternion& q, glm::vec3& rotation)
	{
		double test = q.x()*q.y() + q.z()*q.w();
		if (test > 0.499) { // singularity at north pole
			rotation.y = 2 * atan2(q.x(), q.w());
			rotation.x = PI / 2.0f;
			rotation.z = 0;
			return;
		}
		if (test < -0.499) { // singularity at south pole
			rotation.y = -2 * atan2(q.x(), q.w());
			rotation.x = -PI / 2.0f;
			rotation.z = 0;
			return;
		}
		double sqx = q.x()*q.x();
		double sqy = q.y()*q.y();
		double sqz = q.z()*q.z();
		rotation.y = atan2(2 * q.y()*q.w() - 2 * q.x()*q.z(), 1 - 2 * sqy - 2 * sqz);
		rotation.x = asin(2 * test);
		rotation.z = atan2(2 * q.x()*q.w() - 2 * q.y()*q.z(), 1 - 2 * sqx - 2 * sqz);
	}
	void changeSpeed(float speed, float angle);
public:
	NonPC(double _h, double _r, btShapeManager* _sm, glm::vec3 _spawn, float radius, float height, float mass,
		Model* _bmodel, GLuint _shader, GLuint _text, SoundManager *sound_man /*other parameters*/);
	~NonPC();
	btRigidBody* addBoundingCapsule(float rad, float height, float x, float y, float z, float mass);
	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d);
	void moveNpc(vertex* v, float speed);
	queue<vertex*> findPath(AdjacencyList *adjList, int startId, int endId);

	bool update(Model* modelData, glm::mat4 view, glm::mat4 proj, float dt, Grid* _g, Player *player, GLuint shader, float speed);
	void render(Model * modelData, glm::mat4 view, glm::mat4 proj, GLuint shader, Player *player);

	void setAttack(double atk);
	double getAttack();
	void modifyHealth(double newHp);
	double getRange();
	void setRange(double rng);
	double getHealth();
	void setAttackSpeed(double atkspd);
	double getAttackSpeed();
	void setName(string _name);
	string getName();
	void setState(npcState newState);
	npcState getState();
protected:
	btShapeManager *shapeManager;
	SoundManager *s_manager;
	btRigidBody* npcBody;
	btPairCachingGhostObject* npcGhost;
	Model * boundingModel;
	GLuint texture;
	queue<vertex*> currentPath;
	double recalcTimer = REFRESHRATE;
	double attackTimer = 0;
	glm::vec3 spawn;
	double health;
	double range;
	double attack;
	double attackSpeed;
	bool init = true;
	string name;
	npcState currentState = IDLE;
};

#endif