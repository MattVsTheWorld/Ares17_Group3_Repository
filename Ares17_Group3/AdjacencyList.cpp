#include "AdjacencyList.h"

//////////////////////////
//// Edge class start ////
//////////////////////////

edge::edge(vertex *A, vertex *B, int w) {
	adjacent[0] = A;
	adjacent[1] = B;
	weight = w;
}

vertex** edge::getEndPoints() {
	return this->adjacent;
}

vertex* edge::getNeighbour(vertex* startingVert) {
	if (startingVert->getIndex() == this->adjacent[0]->getIndex())
		return this->adjacent[1];
	else if (startingVert->getIndex() == this->adjacent[1]->getIndex())
		return this->adjacent[0];
}

int edge::getWeight() {
	return this->weight;
}

void edge::displayEdgeInfo() {
	cout << ">Edge with weight: " << this->weight << endl;
	cout << "Edge connects vertex [" << this->adjacent[0]->getIndex() << "]\n";
	this->adjacent[0]->displayVertexInfo();
	cout << "And vertex [" << this->adjacent[1]->getIndex() << "]\n";
	this->adjacent[1]->displayVertexInfo();
}

////////////////////////
//// Edge class end ////
///////////////////////

////////////////////////////
//// Vertex class start ////
///////////////////////////
vertex::vertex(int index, pair<int, int> coords, bool _walkable) {
	this->vertexIndex = index;
	this->vertexCoords = coords;
	this->walkable = _walkable;
	edgeList = new list<edge *>;
}

void vertex::displayVertexInfo() {
	cout << "Index: [" << this->vertexIndex << "] ";
	cout << "Coords: [" << this->vertexCoords.first << "," << this->vertexCoords.second << "]\n";
}

int vertex::getIndex() {
	return this->vertexIndex;
}
bool vertex::addEdge(edge *e) {
	for (const auto edgeIterator : *(this->edgeList))
	{
		if ((static_cast<edge*>(edgeIterator)) == e)
			throw EDGEDUPLICATE;
	}
	this->edgeList->push_back(e);
	return true;
}

list<edge *>* vertex::getEdgeList() {
	return this->edgeList;
}

void vertex::displayEdgeList() {
	cout << ">Vertex [" << this->vertexIndex << "] has edges:\n";
	for (const auto edgeIterator : *(this->edgeList))
		(static_cast<edge*>(edgeIterator))->displayEdgeInfo();
}

double vertex::getfCost() {
	return (this->gCost + this->hCost);
}

double vertex::getgCost() {
	return this->gCost;
}
double vertex::gethCost() {
	return this->hCost;
}

void vertex::setgCost(double _g) {
	this->gCost = _g;
}
void vertex::sethCost(double _h) {
	this->hCost = _h;
}

pair<int, int> vertex::getCoords() {
	return this->vertexCoords;
}

void vertex::setParent(vertex* _parent) {
	this->parent = _parent;
}

vertex* vertex::getParent() {
	return this->parent;
}

//////////////////////////
//// Vertex class end ////
/////////////////////////

///////////////////////////////////
//// AdjacencyList class start ////
//////////////////////////////////
AdjacencyList::AdjacencyList() {
	vertices = new vector<vertex *>;
	vertexNo = 0;
	edgeNo = 0;
}
void AdjacencyList::addVertex(vertex *v) {									// Case specific assumption that added vertices won't take the same position
	if (v->getIndex() > this->vertexNo || v->getIndex() < 0)
		throw OUTOFBOUNDS;
	vertices->push_back(v);
	this->vertexNo++;
}

void AdjacencyList::addEdge(edge *e) {										// Case specific assumption the user won't attempt to enter the same edge twice
	if (e->getEndPoints()[0]->getIndex() >= this->vertexNo
		|| e->getEndPoints()[1]->getIndex() >= this->vertexNo)
		throw OUTOFBOUNDS;

	int endPointsFound = 0;													// counter to be certain edge reference is passed to both vertices it connects
	vector<vertex *>::iterator vectorIterator = this->vertices->begin();
	while (vectorIterator != this->vertices->end() || endPointsFound < 2) {
		if ((static_cast<vertex*>(*vectorIterator))->getIndex() == e->getEndPoints()[0]->getIndex()) {
			(static_cast<vertex*>(*vectorIterator))->addEdge(e);
			endPointsFound++;
		}
		else if ((static_cast<vertex*>(*vectorIterator))->getIndex() == e->getEndPoints()[1]->getIndex()) {
			(static_cast<vertex*>(*vectorIterator))->addEdge(e);
			endPointsFound++;
		}
		vectorIterator++;
	}
	this->edgeNo++;
}

vertex* AdjacencyList::getVertex(int index) {
	if (index >= this->vertexNo || index < 0)
		throw OUTOFBOUNDS;
	vector<vertex *>::iterator vectorIterator = this->vertices->begin();
	while (vectorIterator != this->vertices->end()) {
		if ((static_cast<vertex*>(*vectorIterator))->getIndex() == index)
			return (static_cast<vertex*>(*vectorIterator));
		else
			vectorIterator++;
	}
}

vertex* AdjacencyList::getVertex(pair<int, int> coords) {	
	vector<vertex *>::iterator vectorIterator = this->vertices->begin();
	while (vectorIterator != this->vertices->end()) {
		if ((static_cast<vertex*>(*vectorIterator))->getCoords().first == coords.first 
			&& (static_cast<vertex*>(*vectorIterator))->getCoords().second == coords.second)
			return (static_cast<vertex*>(*vectorIterator));
		else
			vectorIterator++;
	}
	// return unwalkable
	return new vertex(0, make_pair(-999, -999), false);
}

void AdjacencyList::displayVertices() {
	for (const auto verticesIterator : *(this->vertices)) {
		cout << "|| Vertex info ||\n";
		(static_cast<vertex*>(verticesIterator))->displayVertexInfo();
		cout << "|| Edge list ||\n";
		(static_cast<vertex*>(verticesIterator))->displayEdgeList();
		cout << endl;
	}
}

void AdjacencyList::resetCosts() {
	for (const auto &vectorIterator : *(vertices)) {
		vectorIterator->sethCost(0);
		vectorIterator->setgCost(0);
	}
}
/////////////////////////////////
//// AdjacencyList class end ////
////////////////////////////////

