#include "Container.h"


Container::Container(void)
{
	int i =  0;
}

Container::Container(Vector3 pos, Vector3 halfSize, int maxBodies, int myDepth)
{
	this->pos = pos;
	this->halfSize = halfSize;
	this->maxBodies = maxBodies;
	this->myDepth = 0;

	this->maxDepth = 2;

	// Start all the children at NULL
	for(int i = 0; i < 8; i++) 
		children[i] = NULL;
}

Container::~Container(void)
{
	// Recursively destroy octants
	for(int i=0; i<8; ++i) 
		delete children[i];
}

// Determine which octant of the tree would contain 'point'
int Container::getOctantContainingPoint(Body& point)
{
	int oct = 0;

	if(point.pos.x >= pos.x) oct |= 4;
	if(point.pos.y >= pos.y) oct |= 2;
	if(point.pos.z >= pos.z) oct |= 1;


	if(point.pos.x + point.radius >= pos.x && point.pos.x - point.radius <= pos.x)
		oct = 8; // 8 is this oct
	if(point.pos.y + point.radius >= pos.y && point.pos.y - point.radius <= pos.y)
		oct = 8; // 8 is this oct
	if(point.pos.z + point.radius >= pos.z && point.pos.z - point.radius <= pos.z)
		oct = 8; // 8 is this oct


	//if(point.x >= pos.x) 
	//{
	//	if(point.y >= pos.y)
	//	{
	//		if(point.z >= pos.z)
	//			oct = 1;
	//		else
	//			oct = 2;
	//	}
	//	//etc
	//}
	////etc

	if(oct != 8)
	{
		int x = 0;
	}

	return oct;
}

bool Container::isLeafNode()
{
	return children[0] == NULL;
	//return children[0] == nullptr;
}

void Container::Insert(Body* point)
{
	// If this node doesn't have a data point yet assigned 
	// and it is a leaf, then we're done!
	if(isLeafNode() || myDepth == maxDepth)
	{
		if(bodies.size() < maxBodies)
		{
			bodies.push_back(point);
			return;
		} 
		else 
		{
			// We're at a leaf, but there's already max number of bodies here
			// We will split this node so that it has 8 child octants
			// and then insert the old data that was here, along with 
			// this new data point

			// Save this data point that was here for a later re-insert
			std::vector<Body*> oldPoints = bodies;
			bodies.clear();

			// Split the current node and create new empty trees for each
			// child octant.
			for(int i=0; i<8; ++i)
			{
				// Compute new bounding box for this child
				Vector3 newOrigin = pos;
				newOrigin.x += halfSize.x * (i&4 ? .5f : -.5f);
				newOrigin.y += halfSize.y * (i&2 ? .5f : -.5f);
				newOrigin.z += halfSize.z * (i&1 ? .5f : -.5f);
				children[i] = new Container(newOrigin, halfSize * 0.5f, maxBodies, myDepth + 1);
			}

			// Re-insert the old points, and insert this new point
			// (We wouldn't need to insert from the root, because we already
			// know it's guaranteed to be in this section of the tree)
			for(int i = 0; i < oldPoints.size(); i++)
			{
				// Check octant and sort into children or self
				int octant = getOctantContainingPoint(*oldPoints[0]);
				if(octant != 8)
					children[octant]->Insert(oldPoints[i]);
				else
					Place(point);
			}
			int octant = getOctantContainingPoint(*point);
			if(octant != 8)
				children[octant]->Insert(point);
			else
				Place(point);
			
		}
	}
	else
	{
		// We are at an interior node. Insert recursively into the 
		// appropriate child octant
		int octant = getOctantContainingPoint(*point);
		if(octant != 8)
			children[octant]->Insert(point);
		else
			Place(point);
	}
}

// Push a body into this container/octant
void Container::Place(Body* point)
{
	this->bodies.push_back(point);
}

// This is a really simple routine for querying the tree for points
// within a bounding box defined by min/max points (bmin, bmax)
// All results are pushed into 'results'
void Container::getPointsInsideBox(Vector3 bmin, Vector3 bmax, std::vector<Body*>& results)
{
	// If we're at a leaf node, just see if the current data point is inside
	// the query bounding box
	if(isLeafNode())
	{
		if(bodies.size() > 0)
		{
			const Vector3& p = bodies[0]->pos;
			if(p.x>bmax.x || p.y>bmax.y || p.z>bmax.z) return;
			if(p.x<bmin.x || p.y<bmin.y || p.z<bmin.z) return;
			results.push_back(bodies[0]);
		}
	}
	else
	{
		// We're at an interior node of the tree. We will check to see if
		// the query bounding box lies outside the octants of this node.
		for(int i=0; i<8; ++i)
		{
			// Compute the min/max corners of this child octant
			Vector3 cmax = children[i]->pos + children[i]->halfSize;
			Vector3 cmin = children[i]->pos - children[i]->halfSize;

			// If the query rectangle is outside the child's bounding box, 
			// then continue
			if(cmax.x<bmin.x || cmax.y<bmin.y || cmax.z<bmin.z) continue;
			if(cmin.x>bmax.x || cmin.y>bmax.y || cmin.z>bmax.z) continue;

			// At this point, we've determined that this child is intersecting 
			// the query bounding box
			children[i]->getPointsInsideBox(bmin,bmax,results);
		} 
	}
}

void Container::getBodiesToCheck(std::vector<Body*>& results)
{
	// if no children && numbodies > 1 - return my bodies
	// else if no children && numbodies <= 1 - return null (no checks please)
	// else if children && numbodies > 0
		// return my bodies, and all children and children's children, etc (recursively)

	// if no children and some bodies
		// collide my bodies against each other
		// collide my bodies against provided bodies
	// if no children and no bodies
		// return;
	// if children
		// iterate over children
			// if some bodies
				// add my bodies and recurse on this child
			// else
				// recurse on this child
			// clear out bodies from child before iterating next child

	if(children[0] == NULL && bodies.size() > 0)
	{
		// collide my bodies against each other
		for(int i = 0; i < bodies.size(); i++)
		{
			for(int j = i + 1; j < bodies.size(); j++)
			{
				PlanetCollRes(*bodies[i], *bodies[j]);
				std::cout << "checked collisions with my body and my other body" << std::endl;
			}
		}

		// see if any of the provided bodies collide with any of my bodies
		for(int i = 0; i < results.size(); i++)
		{
			for(int j = 0; j < bodies.size(); j++)
			{
				PlanetCollRes(*bodies[i], *bodies[j]);
				std::cout << "checked collisions with provided body against my body" << std::endl;
			}
		}
	}
	if(children[0] == NULL && bodies.size() <= 0)
	{
		std::cout << "this octant had no children or bodies" << std::endl;
		return;
	}
	if(children[0] != NULL)
	{
		for(int j = 0; j < 8; j++)
		{
			if(bodies.size() > 0)
			{
				for(int i = 0; i < bodies.size(); i++)
					results.push_back(bodies[i]);
				children[j]->getBodiesToCheck(results);
			}
			else
				children[j]->getBodiesToCheck(results);
			//results.erase(results.end() - children[j]->bodies.size(), results.end());
		}
	}
}

// Based off of sphere collision from - 
void Container::PlanetCollRes(Body& a, Body& b)
{
	float dist = Vector3::dist(a.pos, b.pos);
	if (dist < a.radius + b.radius && dist != 0) //they are intersecting	// if different objects are on the same pos?
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