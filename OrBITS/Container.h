#ifndef CONTAINER_H
#define CONTAINER_H

#include "Vector3.h"
#include "Body.h"

#include <vector>

class Container
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