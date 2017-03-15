#pragma once
#ifndef GRID
#define GRID
#include "Node.h"

//Node size = 2x2 seems fair

class Grid {
public:
	Grid(){
		for (int i = -gridSize/2; i < gridSize/2; i+=2)
			for (int j = -gridSize/2; j < gridSize/2; j+=2)
				grid[i][j] = new Node(true, glm::vec3(i, 0, j));
	}
protected:
private:
	glm::vec2 gridWorldSize;
	float nodeMargin = 2;
	static const int gridSize = 100;
	Node* grid[gridSize][gridSize];
};

#endif