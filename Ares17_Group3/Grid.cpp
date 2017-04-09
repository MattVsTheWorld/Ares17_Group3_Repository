#include "Grid.h"

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
				//		cout << IDgrid[foo][bar] << " ";
			}
			catch (int e) {
				cout << "Exception occured. Exception Nr:" << e << '.\n';
			}
			bar++;
		}
		bar = 0;
		foo++;
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

	//for (const auto &it : grid)
	//	it
	//grid[i][j] = new Node(true, glm::vec3(i, 0, j));

	//grid->displayVertices();
	//grid->getVertexNo()
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
	//cout << x << endl;
	int y = (((gridHalfSide * 2)) / DEFAULTRADIUS) * percentY;
	//cout << "\n|||||||||\n";
	//cout << "percentX: " << percentX << "| percentY: " << percentY << " ";
	//cout << "x: " << x << "| y: " << y << " ";
	//cout << "ID: " << IDgrid[x][y];
	//cout << "\n|||||||||\n";
	return IDgrid[x][y];
}


AdjacencyList*  Grid::getAdjList() {
	return grid;
}