#include "Octree.h"


//Constructs a new Octree.  c1 is (minX, minY, minZ), c2 is (maxX, maxY,
//maxZ), and d is the depth, which starts at 1.
Octree::Octree(Vector3 c1, Vector3 c2, int d)
{
	corner1 = c1;
	corner2 = c2;
	center = (c1 + c2) / 2;
	depth = d;
	numBalls = 0;
	hasChildren = false;
	TIME_BETWEEN_UPDATES = 0.01f;
}

//Destructor
Octree::~Octree() {
	if (hasChildren) {
		destroyChildren();
	}
}

//Adds a ball to or removes one from the children of this
void Octree::fileBody(Body* ball, Vector3 pos, bool addBall)
{
	//Figure out in which child(ren) the ball belongs
	for(int x = 0; x < 2; x++) {
		if (x == 0) {
			if (pos[0] - ball->radius > center[0]) {
				continue;
			}
		}
		else if (pos[0] + ball->radius < center[0]) {
			continue;
		}

		for(int y = 0; y < 2; y++) {
			if (y == 0) {
				if (pos[1] - ball->radius > center[1]) {
					continue;
				}
			}
			else if (pos[1] + ball->radius < center[1]) {
				continue;
			}

			for(int z = 0; z < 2; z++) {
				if (z == 0) {
					if (pos[2] - ball->radius > center[2]) {
						continue;
					}
				}
				else if (pos[2] + ball->radius < center[2]) {
					continue;
				}

				//Add or remove the ball
				if (addBall) {
					children[x][y][z]->add(ball);
				}
				else {
					children[x][y][z]->remove(ball, pos);
				}
			}
		}
	}
}

//Creates children of this, and moves the balls in this to the children
void Octree::haveChildren()
{
	for(int x = 0; x < 2; x++) {
		float minX;
		float maxX;
		if (x == 0) {
			minX = corner1[0];
			maxX = center[0];
		}
		else {
			minX = center[0];
			maxX = corner2[0];
		}

		for(int y = 0; y < 2; y++) {
			float minY;
			float maxY;
			if (y == 0) {
				minY = corner1[1];
				maxY = center[1];
			}
			else {
				minY = center[1];
				maxY = corner2[1];
			}

			for(int z = 0; z < 2; z++) {
				float minZ;
				float maxZ;
				if (z == 0) {
					minZ = corner1[2];
					maxZ = center[2];
				}
				else {
					minZ = center[2];
					maxZ = corner2[2];
				}

				children[x][y][z] = new Octree(Vector3(minX, minY, minZ),
					Vector3(maxX, maxY, maxZ),
					depth + 1);
			}
		}
	}

	//Remove all balls from "balls" and add them to the new children
	for(std::vector<Body*>::iterator it = bodies.begin(); it != bodies.end();
		it++) {
			Body* ball = *it;
			fileBody(ball, ball->pos, true);
	}
	bodies.clear();

	hasChildren = true;
}

//Adds all bodies in this or one of its descendants to the specified set
void Octree::collectBodies(std::vector<Body*> &bs)
{
	if (hasChildren) {
		for(int x = 0; x < 2; x++) {
			for(int y = 0; y < 2; y++) {
				for(int z = 0; z < 2; z++) {
					children[x][y][z]->collectBodies(bs);
				}
			}
		}
	}
	else {
		for(std::vector<Body*>::iterator it = bodies.begin(); it != bodies.end();
			it++) {
				Body* body = *it;
				bs.push_back(body);
		}
	}
}

//Destroys the children of this, and moves all balls in its descendants
//to the "balls" set
void Octree::destroyChildren()
{
	//Move all bodies in descendants of this to the "bodies" vector
	collectBodies(bodies);

	for(int x = 0; x < 2; x++) {
		for(int y = 0; y < 2; y++) {
			for(int z = 0; z < 2; z++) {
				delete children[x][y][z];
			}
		}
	}

	hasChildren = false;
}

//Removes the specified body at the indicated position
void Octree::remove(Body* body, Vector3 pos)
{
	numBalls--;

	if (hasChildren && numBalls < MIN_BODIES_PER_OCTREE) {
		destroyChildren();
	}

	if (hasChildren) {
		fileBody(body, pos, false);
	}
	else
	{
		std::vector<Body*>::iterator position = std::find(bodies.begin(), bodies.end(), body);
		if (position != bodies.end()) // == vector.end() means the element was not found
			bodies.erase(position);
	}
}

//Adds a body to this
void Octree::add(Body* body) {
	numBalls++;
	if (!hasChildren && depth < MAX_OCTREE_DEPTH &&
		numBalls > MAX_BODIES_PER_OCTREE) {
			haveChildren();
	}

	if (hasChildren) {
		fileBody(body, body->pos, true);
	}
	else 
	{
		bodies.push_back(body);
	}
}
	
//Removes a body from this
void Octree::remove(Body* body)
{
	remove(body, body->pos);
}

//Changes the position of a body in this from oldPos to body->pos
void Octree::bodyMoved(Body* body, Vector3 oldPos) {
	remove(body, oldPos);
	add(body);
}

//Advances the state of the balls by t.  timeUntilUpdate is the amount of time
//until the next call to performUpdate.
void Octree::advance(std::vector<Body*> &bodies, Octree* octree, float t, float& timeUntilUpdate)
{
//	while (t > 0) {
	//	if (timeUntilUpdate <= t) {
			moveBodies(bodies, octree, timeUntilUpdate);
			performUpdate(bodies, octree);
	/*		t -= timeUntilUpdate;
			timeUntilUpdate = TIME_BETWEEN_UPDATES;
		}
		else {
			moveBalls(balls, octree, t);
			timeUntilUpdate -= t;
			t = 0;
		}
	}*/
}

//Adds potential body-body collisions to the specified set
void Octree::potentialBodyBodyCollisions(std::vector<BallPair> &collisions) {
	if (hasChildren) {
		for(int x = 0; x < 2; x++) {
			for(int y = 0; y < 2; y++) {
				for(int z = 0; z < 2; z++) {
					children[x][y][z]->
						potentialBodyBodyCollisions(collisions);
				}
			}
		}
	}
	else {
		//Add all pairs (ball1, ball2) from bodies
		for(std::vector<Body*>::iterator it = bodies.begin(); it != bodies.end(); it++) {
				Body* ball1 = *it;
				for(std::vector<Body*>::iterator it2 = bodies.begin();
					it2 != bodies.end(); it2++) {
						Body* ball2 = *it2;
						//This test makes sure that we only add each pair once
						if (ball1 < ball2) {
							BallPair bp;
							bp.ball1 = ball1;
							bp.ball2 = ball2;
							collisions.push_back(bp);
						}
				}
		}
	}
}

void Octree::potentialBodyBodyCollisions(std::vector<BallPair> &potentialCollisions, std::vector<Body*> &bodies, Octree* octree)
{
	//Fast method
	octree->potentialBodyBodyCollisions(potentialCollisions);
	
	/*
	//Slow method
	for(unsigned int i = 0; i < balls.size(); i++) {
		for(unsigned int j = i + 1; j < balls.size(); j++) {
			BallPair bp;
			bp.ball1 = balls[i];
			bp.ball2 = balls[j];
			potentialCollisions.push_back(bp);
		}
	}
	*/
}

//Moves all of the balls by their velocity times dt
void Octree::moveBodies(std::vector<Body*> &bodies, Octree* octree, float dt)
{
	for(unsigned int i = 0; i < bodies.size(); i++) {
		Body* body = bodies[i];
		Vector3 oldPos = body->pos;
		octree->bodyMoved(body, oldPos);
	}
}

//Returns whether two balls are colliding
bool Octree::testBodyBodyCollision(Body* b1, Body* b2) {
	//Check whether the balls are close enough
	float r = b1->radius + b2->radius;
	if ( (b1->pos - b2->pos).length(b1->pos - b2->pos) * (b1->pos - b2->pos).length(b1->pos - b2->pos) < r * r) {
		//Check whether the balls are moving toward each other
		Vector3 netVelocity = b1->vel - b2->vel;
		Vector3 displacement = b1->pos - b2->pos;
		return netVelocity.dot(netVelocity, displacement) < 0;
	}
	else
		return false;
}

//Handles all ball-ball collisions
void Octree::handleBodyBodyCollisions(std::vector<Body*> &bodies, Octree* octree) {
	std::vector<BallPair> bps;
	potentialBodyBodyCollisions(bps, bodies, octree);
	for(unsigned int i = 0; i < bps.size(); i++) {
		BallPair bp = bps[i];
		
		Body* b1 = bp.ball1;
		Body* b2 = bp.ball2;

		PlanetCollRes(*b1, *b2);
		
		//if (testBallBallCollision(b1, b2)) {
		//	//Make the balls reflect off of each other
		//	Vector3 displacement = (b1->pos - b2->pos).normalize(b1->pos - b2->pos);
		//	b1->vel -= displacement * b1->vel.dot(b2->vel, displacement) * 2;
		//	b2->vel -= displacement * b2->vel.dot(b2->vel, displacement) * 2;
		//}
	}
}

//Applies gravity and handles all collisions.  Should be called every
//TIME_BETWEEN_UPDATES seconds.
void Octree::performUpdate(std::vector<Body*> &balls, Octree* octree) {
	//applyGravity(balls);
	handleBodyBodyCollisions(balls, octree);
	//handleBallWallCollisions(balls, octree);
}

// Based off of sphere collision from - http://www.gamasutra.com/view/feature/131424/pool_hall_lessons_fast_accurate_.php?page=3
void Octree::PlanetCollRes(Body& a, Body& b)
{
	float dist = Vector3::dist(a.pos, b.pos);
	if (dist < a.radius + b.radius) //they are intersecting
	{
		float overlap = (a.radius + b.radius) - dist;
		Vector3 toB = b.pos - a.pos;

		// Fixes pos, so balls are just barely contacting
		Vector3 move = Vector3::normalize(toB);
		if ((a.type == SUN && b.type != SUN) || (a.type == PLANET && b.type == ASTEROID)) // ONLY MOVE 'B'
		{
			move *= overlap;
			b.pos += move;
		}
		else if ((b.type == SUN && a.type != SUN) || (b.type == PLANET && a.type == ASTEROID)) // ONLY MOVE 'A'
		{
			move *= -overlap;
			a.pos += move;
		}
		else // MOVE BOTH
		{
			move *= overlap / 2.f;
			b.pos += move;
			move *= -1;
			a.pos += move;
		}

		// First, find the normalized vector n from the center of 
		// circle1 to the center of circle2
		Vector3 n = b.pos - a.pos; //c1, c2
		n = Vector3::normalize(n);
		// Find the length of the component of each of the movement
		// vectors along n. 
		// a1 = v1 . n
		// a2 = v2 . n
		float a1 = Vector3::dot(b.vel,n);
		float a2 = Vector3::dot(a.vel,n);

		// Using the optimized version, 
		// optimizedP =  2(a1 - a2)
		//              -----------
		//                m1 + m2
		float optimizedP = (2.0f * (a1 - a2)) / (b.mass + a.mass);

		// Calculate v1', the new movement vector of circle1
		// v1' = v1 - optimizedP * m2 * n
		Vector3 bNew = b.vel - (n * (optimizedP * a.mass));
		b.vel = bNew;

		// Calculate v1', the new movement vector of circle1
		// v2' = v2 + optimizedP * m1 * n
		Vector3 aNew = a.vel + (n * (optimizedP * b.mass));
		a.vel = aNew;
	}
}