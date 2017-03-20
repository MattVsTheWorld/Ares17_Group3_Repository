#pragma once
#ifndef GRID
#define GRID
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "AdjacencyList.h"
#define DEFAULTWEIGHT 2.0f
#define DEFAULTRADIUS 5

//Node size = 2x2 seems fair

// Adj list; all nodes from -35 to +35, all 9 around neighbours
// for / for /
	//	add vertex
// add neighbours after
// right = i + size*2j
class Grid {
public:
	Grid() {  /* PARAMS */
		grid = new AdjacencyList();

		int index = 0;
		pair<int, int> coordinates;
		for (int i = -gridHalfSide; i <= gridHalfSide; i += DEFAULTRADIUS) {
			for (int j = -gridHalfSide; j <= gridHalfSide; j += DEFAULTRADIUS)
			{

				coordinates.first = i;
				coordinates.second = j;
				vertex *newVertex = new vertex(index, coordinates, true);
				index++;
				try {
					grid->addVertex(newVertex);
				}
				catch (int e) {
					cout << "Exception occured. Exception Nr:" << e << '.\n';
				}
			}
		}

	//	int poof = 0;
		for (int i = 0; i < index; i++)
		{
			try {
				vector <edge*> newEdges;

				newEdges.push_back(new edge(grid->getVertex(i), grid->getVertex(make_pair((grid->getVertex(i)->getCoords().first + DEFAULTRADIUS), (grid->getVertex(i)->getCoords().second))), DEFAULTWEIGHT));
				newEdges.push_back(new edge(grid->getVertex(i), grid->getVertex(make_pair((grid->getVertex(i)->getCoords().first + DEFAULTRADIUS), (grid->getVertex(i)->getCoords().second + DEFAULTRADIUS))), DEFAULTWEIGHT));
				newEdges.push_back(new edge(grid->getVertex(i), grid->getVertex(make_pair((grid->getVertex(i)->getCoords().first + DEFAULTRADIUS), (grid->getVertex(i)->getCoords().second - DEFAULTRADIUS))), DEFAULTWEIGHT));
				newEdges.push_back(new edge(grid->getVertex(i), grid->getVertex(make_pair((grid->getVertex(i)->getCoords().first - DEFAULTRADIUS), (grid->getVertex(i)->getCoords().second))), DEFAULTWEIGHT));
				newEdges.push_back(new edge(grid->getVertex(i), grid->getVertex(make_pair((grid->getVertex(i)->getCoords().first - DEFAULTRADIUS), (grid->getVertex(i)->getCoords().second + DEFAULTRADIUS))), DEFAULTWEIGHT));
				newEdges.push_back(new edge(grid->getVertex(i), grid->getVertex(make_pair((grid->getVertex(i)->getCoords().first - DEFAULTRADIUS), (grid->getVertex(i)->getCoords().second - DEFAULTRADIUS))), DEFAULTWEIGHT));
				newEdges.push_back(new edge(grid->getVertex(i), grid->getVertex(make_pair((grid->getVertex(i)->getCoords().first), (grid->getVertex(i)->getCoords().second + DEFAULTRADIUS))), DEFAULTWEIGHT));
				newEdges.push_back(new edge(grid->getVertex(i), grid->getVertex(make_pair((grid->getVertex(i)->getCoords().first), (grid->getVertex(i)->getCoords().second - DEFAULTRADIUS))), DEFAULTWEIGHT));

				for (const auto &it : newEdges)
					grid->getVertex(i)->addEdge(it);

			//	cout << poof++ << " ";
			}
			catch (int e) {
				cout << "Exception occured. Exception Nr:" << e << '.\n';
			}
		}

		cout << "Index no :" << index << endl;
		//for (const auto &it : grid)
		//	it
				//grid[i][j] = new Node(true, glm::vec3(i, 0, j));

		//grid->displayVertices();
		//grid->getVertexNo()
	}

	AdjacencyList* getAdjList() {
		return grid;
	}
protected:
private:
	//	glm::vec2 gridWorldSize;
	//	float nodeMargin = 2;
	static const int gridHalfSide = 35; //TODO change grid size
	AdjacencyList *grid;
};

#endif