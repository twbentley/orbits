#ifndef CONTAINER_H
#define CONTAINER_H

#include "Cube.h"
#include "Body.h"

#include <vector>

class Container : Cube
{
public:
	Vector3 pos;
	Vector3 size;
	std::vector<Body> bodies;


	Container(void);
	Container(Vector3 pos, Vector3 size);
	~Container(void);
};

#endif