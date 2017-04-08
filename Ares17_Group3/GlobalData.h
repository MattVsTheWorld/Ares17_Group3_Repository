#pragma once
#include "SceneManager.h"
#include <memory>

#ifndef GLOBALDATA
#define GLOBALDATA

#define AMBIENT_FACTOR 1.0f
#define DIFFUSE_FACTOR 0.9f
#define SPECULAR_FACTOR 1.0f
#define ATTENUATION_CONST 0.035f
#define ATTENUATION_LINEAR 0.0065f
#define ATTENUATION_QUAD 0.0024f

struct PointLight {
	glm::vec3 position;

	float att_constant;
	float att_linear;
	float att_quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

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

		mainLight = {
			glm::vec3(0.0f, 25.0f, 0.0f),

			ATTENUATION_CONST, ATTENUATION_LINEAR, ATTENUATION_QUAD,

			glm::vec3(AMBIENT_FACTOR,AMBIENT_FACTOR,AMBIENT_FACTOR),
			glm::vec3(DIFFUSE_FACTOR,DIFFUSE_FACTOR,DIFFUSE_FACTOR),
			glm::vec3(SPECULAR_FACTOR,SPECULAR_FACTOR,SPECULAR_FACTOR)
		};
	}
	//Player
	Player*player;

	//Managers
	hudManager *h_manager;
	Skybox *skybox;
	btShapeManager *bt_manager;
	Projectile *projectile_manager;
	SoundManager *sound_manager;

	//AI
	Grid* level1Grid;

	PointLight mainLight;

};

#endif