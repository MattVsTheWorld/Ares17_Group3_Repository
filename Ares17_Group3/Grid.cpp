#include "Grid.h"

// Not really a great implementation to do this
Grid::Grid() {
	grid = new AdjacencyList();

	int index = 0;
	int foo = 0;
	int bar = 0;
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
				IDgrid[foo][bar] = index;
			}
			catch (int e) {
				cout << "Exception occured. Exception Nr:" << e << '.\n';
			}
			bar++;
		}
		bar = 0;
		foo++;
	}
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
		}
		catch (int e) {
			cout << "Exception occured. Exception Nr:" << e << '.\n';
		}
	}
}

int Grid::getNodeFromWorldPos(btVector3 &pos) {
	float percentX = (pos.x() + gridHalfSide) / (gridHalfSide * 2);
	float percentY = (pos.z() + gridHalfSide) / (gridHalfSide * 2);
	// If outside grid... clamp
	if (percentX > 1)
		percentX = 1;
	if (percentY > 1)
		percentY = 1;
	if (percentX < 0)
		percentX = 0;
	if (percentY < 0)
		percentY = 0;

	int x = (((gridHalfSide * 2) / DEFAULTRADIUS)) * percentX;
	int y = (((gridHalfSide * 2)) / DEFAULTRADIUS) * percentY;
	return IDgrid[x][y];
}


AdjacencyList*  Grid::getAdjList() {
	return grid;
}