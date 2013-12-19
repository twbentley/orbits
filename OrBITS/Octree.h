#ifndef OCTREE_H
#define OCTREE_H

#include "Vector3.h"
#include "Body.h"

#include <algorithm>

#include <vector>

class Octree
{
	static const int MAX_OCTREE_DEPTH = 7;
	static const int MIN_BODIES_PER_OCTREE = 3;
	static const int MAX_BODIES_PER_OCTREE = 30;
	float TIME_BETWEEN_UPDATES;

	//Stores a pair of balls
	struct BallPair
	{
		Body* ball1;
		Body* ball2;
	};

public:
		Vector3 corner1; //(minX, minY, minZ)
		Vector3 corner2; //(maxX, maxY, maxZ)
		Vector3 center;//((minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2)

		Octree *children[2][2][2];
		bool hasChildren;
		std::vector<Body*> bodies;
		int depth;
		int numBalls;

		void fileBody(Body* ball, Vector3 pos, bool addBall);
		void haveChildren();
		void collectBodies(std::vector<Body*> &bs);
		void destroyChildren();
		void remove(Body* ball, Vector3 pos);

//	public:
		Octree(Vector3 c1, Vector3 c2, int d);
		~Octree();

		void add(Body* ball);
		void remove(Body* ball);
		void bodyMoved(Body* ball, Vector3 oldPos);
		void potentialBodyBodyCollisions(std::vector<BallPair> &collisions);
		void potentialBodyBodyCollisions(std::vector<BallPair> &potentialCollisions, std::vector<Body*> &balls, Octree* octree);
		void moveBodies(std::vector<Body*> &balls, Octree* octree, float dt);
		bool testBodyBodyCollision(Body* b1, Body* b2);
		void handleBodyBodyCollisions(std::vector<Body*> &balls, Octree* octree);
		void performUpdate(std::vector<Body*> &balls, Octree* octree);
		void advance(std::vector<Body*> &balls, Octree* octree, float t, float& timeUntilUpdate);

		void PlanetCollRes(Body& a, Body& b);
};

#endif