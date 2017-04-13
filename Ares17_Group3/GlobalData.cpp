#include "GlobalData.h"

GlobalData::GlobalData(glm::vec3 eye) {
	shadows = false;
	h_manager = new hudManager(shadows);
	bt_manager = new btShapeManager();
	player = new Player(eye, 1.0f, 1.5f, 80.0f, bt_manager);
	projectile_manager = new Projectile(bt_manager);
	sound_manager = new SoundManager();



	mainLight = {
		glm::vec3(0.0f, 25.0f, 0.0f),

		ATTENUATION_CONST, ATTENUATION_LINEAR, ATTENUATION_QUAD,

		glm::vec3(AMBIENT_FACTOR,AMBIENT_FACTOR,AMBIENT_FACTOR),
		glm::vec3(DIFFUSE_FACTOR,DIFFUSE_FACTOR,DIFFUSE_FACTOR),
		glm::vec3(SPECULAR_FACTOR,SPECULAR_FACTOR,SPECULAR_FACTOR)
	};
}