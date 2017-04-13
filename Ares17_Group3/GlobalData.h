#pragma once
#include "SceneManager.h"
#include <memory>

#ifndef GLOBALDATA
#define GLOBALDATA

#define AMBIENT_FACTOR 2.0f
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
	GlobalData(glm::vec3 eye);
	//Player
	Player*player;
	//Managers
	hudManager *h_manager;
	Skybox *skybox;
	btShapeManager *bt_manager;
	Projectile *projectile_manager;
	SoundManager *sound_manager;
	bool shadows;
	PointLight mainLight;
};

#endif