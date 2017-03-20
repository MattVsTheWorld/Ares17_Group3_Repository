#pragma once
#ifndef A_STAR
#define A_STAR
#include <iostream>
#include <queue>
#include <functional>
#include "AdjacencyList.h"
#include <unordered_set>
using namespace std;

// A* search class: attempts to find best path between two points of a given graph
class A_star {																
private:
	vector<vertex*> openSet;												// vertices being considered as possible part of best path
	unordered_set<vertex*> closedSet;										// vertices determined to be either nonoptimal for the route or certainly part of it
	AdjacencyList* adjList;													// graph structure to traverse
	int iterations;															// number of while iterations (roughly translates to vertices visited) before the goal is found
	// Methods used by other class instances
	list<vertex*> retracePath(vertex* _start, vertex* _end);				// retrace the path through the use of parent references
	double heuristic(pair<int, int> coord_A, pair<int, int> coord_B);		// calculate heuristic (in this case, euclidean distance)
	protected:
public:
	A_star(AdjacencyList *_adjList);										// Create base object by copying the graph
	list<vertex*> algorithm_standard(vertex* start, vertex* end);			// "standard" version of the algorithm (every iteration explores the entire open set)
	int getIterationCount();												// return iteration count
	void resetIterationCount();												// set iteration count to 0
};
#endif