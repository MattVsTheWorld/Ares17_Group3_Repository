#pragma once
#ifndef NODE
#define NODE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Node {
public:
	Node(bool _walkable, glm::vec3 _worldPos)
	{
		walkable = _walkable;
		worldPos = _worldPos;
	}
protected:
private:
	bool walkable;
	glm::vec3 worldPos;
};

#endif