#pragma once
#ifndef GRID
#define GRID
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "AdjacencyList.h"
#include "btBulletDynamicsCommon.h"
#define DEFAULTWEIGHT 10.0f
#define DEFAULTRADIUS 5

class Grid {
public:
	Grid();
	int getNodeFromWorldPos(btVector3 &pos);
	AdjacencyList* getAdjList();
protected:
private:

	static const int gridHalfSide = 40; // default size
	int IDgrid[((gridHalfSide * 2) / DEFAULTRADIUS) + 1 ][((gridHalfSide * 2) / DEFAULTRADIUS) + 1];
	AdjacencyList *grid;
};

#endif