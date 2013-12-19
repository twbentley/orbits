#ifndef CONTAINER_H
#define CONTAINER_H

#include "Cube.h"
#include "Body.h"

#include <vector>

class Container : Cube
{
public:
	Vector3 pos;
	Vector3 halfSize;
	int maxBodies;
	int myDepth;
	int maxDepth;
	std::vector<Body*> bodies;
	Container* children[8];

	Container(void);
	Container(Vector3 pos, Vector3 halfSize, int maxBodies, int myDepth);
	~Container(void);

	int getOctantContainingPoint(Body& point);
	bool isLeafNode();
	void Insert(Body* point);
	void Place(Body* point);
	void getPointsInsideBox(Vector3 bmin, Vector3 bmax, std::vector<Body*>& results);
	void getBodiesToCheck(std::vector<Body*>& results);
	void PlanetCollRes(Body& a, Body& b);
};

#endif