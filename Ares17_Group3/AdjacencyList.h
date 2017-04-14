#pragma once
#ifndef ADJACENCYLIST
#define ADJACENCYLIST
#include <list>
#include <vector>
#include <iostream>
using namespace std;

/* 
* Adjacency list class
* Constains the vertex and edge supporting classes, which create instances of vertices (nodes) and edges
* Vertices don't have direct knowledge of neighbours but instead contain a list of edges which point to the neighbours
*/

class vertex;										// class declaration

#define OUTOFBOUNDS	  1								// Attempt to access index out of bounds (unexisting vertex; edge connecting one or two unexsting vertex; etc.)	
#define EDGEDUPLICATE 2								// Attempt to add duplicate edge to vertex edge list

class edge {										// EDGE CLASS: represents path between two vertices
private:
	vertex *adjacent[2];							// Stores pointers to the two vertices connected by this edge
	int	weight;										// The weight value to traverse this edge
protected:
public:
	edge(vertex *A, vertex *B, int w);				// create edge object
	vertex** getEndPoints();						// return end points (pointers to two connected vertices)
	vertex* getNeighbour(vertex* startingVert);		// return neighbour (given one of the end points, return the other of this edge)
	int getWeight();								// return weight of current edge
	void displayEdgeInfo();							// display edge information (weight, index of two connected vertices)

};

class vertex {										// VERTEX CLASS: represents a position (often called node) in the graph
private:
	int vertexIndex;								// identifier of a vertex
	list<edge *> *edgeList;							// pointer to list of edges of current vertex. Neighbouring nodes are being pointed at by these edges
	double gCost;									// A* supporting variable; g contains the overall path cost from starting node to this vertex
	double hCost;									// A* supporting variable; h contains the value of the heuristic
	pair <int, int> vertexCoords;					// Coordinates of vertex in the graph
	vertex* parent;									// A* supporting variable; points to previous vertex visited in the current path
	bool walkable;
protected:
public:
	vertex(int index, pair<int, int> coords, bool _walkable);		// create vertex object
	void displayVertexInfo();						// display vertex information
	int getIndex();									// return index number of the current vertex
	bool addEdge(edge *e);							// add edge to current vertex's edge list
	list<edge *> *getEdgeList();					// return the edge list of the current vertex
	void displayEdgeList();							// display list of edges and corresponding neighbours
	double getfCost();								// return f cost (calculated as sum of g + h values)
	double getgCost();								// return g cost (overall path cost)
	double gethCost();								// return h cost (heuristic)
	void setgCost(double _g);						// set overall path value g
	void sethCost(double _h);						// set heuristic value h 
	pair<int, int> getCoords();						// return coordinates of current vertex
	void setParent(vertex* _parent);				// set parent of current vertex
	vertex* getParent();							// return the parent of current vertex
	bool isWalkable() {
		return this->walkable;
	}
};

class AdjacencyList {								// ADJACENCY LIST: data structure to contain graph data
private:
	int vertexNo;									// Overall number of vertices in the graph
	int edgeNo;										// Overall number of edges in the graph
	vector<vertex *> *vertices;						// Container of all vertices for quick access
protected:
public:
	AdjacencyList();								// create adjacency list object
	void addVertex(vertex *v);						// add vertex to adjacency list
	void addEdge(edge *e);							// add edge to edge list of BOTH vertices it connects
	vertex* getVertex(int index);					// return vertex at given index
	vertex* getVertex(pair<int, int> coords);	// return vertex at given position
	void displayVertices();							// display elements of the adjacency list (vertices and edges)
	void resetCosts();								// reset g and h costs for (e.g. for a second search)

};
#endif
