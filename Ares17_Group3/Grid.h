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

//Node size = 2x2 seems fair

// Adj list; all nodes from -35 to +35, all 9 around neighbours
// for / for /
	//	add vertex
// add neighbours after
// right = i + size*2j
class Grid {
public:
	Grid();
	int getNodeFromWorldPos(btVector3 &pos);
	AdjacencyList* getAdjList();
protected:
private:
	//	glm::vec2 gridWorldSize;
	//	float nodeMargin = 2;
	static const int gridHalfSide = 40; //TODO change grid size
	int IDgrid[((gridHalfSide * 2) / DEFAULTRADIUS) + 1 ][((gridHalfSide * 2) / DEFAULTRADIUS) + 1];
	AdjacencyList *grid;
};

#endif