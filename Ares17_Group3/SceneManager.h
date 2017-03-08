#ifndef SCENEMANAGER
#define SCENEMANAGER

#include "SDL.h"

#include <stack>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SDLmanager.h"
#include "hudManager.h"
#include "Skybox.h"

#include "btShapeManager.h"
#include "Player.h"
// TEMPORARY
#include "loadBitmap.h"
//+++
#include "SoundManager.h"
#include <time.h>
#include "Projectile.h"
#include "NPCDecorator.h"
//+++
#define DEG_TO_RADIAN 0.017453293f
#define PI 3.14159265359f

#define COOL_TIME 0.2f
#define MODEL_SCALING 0.001f
#define BOUNDING_SCALING 0.01f


#define SPEED_CAP_XZ 8.0
#define FRICTION 8.0
#define SPEED_CAP_Y 3.0

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

namespace SceneManager {
	void init(void);
	void update(SDL_Window *window, SDL_Event sdlEvent);
	//fps counter; 3 of 5
	void draw(SDL_Window *window);//, int fps);
}

#endif