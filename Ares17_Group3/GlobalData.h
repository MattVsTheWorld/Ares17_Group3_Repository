#pragma once
#include "SceneManager.h"
#include <memory>


#ifndef GLOBALDATA
#define GLOBALDATA

class GlobalData {
public:
	GlobalData(glm::vec3 eye) {
		player = new Player(eye);
		h_manager = new hudManager();
		bt_manager = new btShapeManager();
		projectile_manager = new Projectile(bt_manager);
		sound_manager = new SoundManager();

		// AI
		level1Grid = new Grid();
	}
	//Player
	Player*player;
	btRigidBody* playerBody;

	//Managers
	hudManager *h_manager;
	Skybox *skybox;
	btShapeManager *bt_manager;
	Projectile *projectile_manager;
	SoundManager *sound_manager;

	//AI
	Grid* level1Grid;

};

#endif