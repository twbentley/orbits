#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

class Sphere :
	public Shape
{
public:
	struct Ring
	{
		std::vector<Vector3> points;
	};

	GLuint nRings;
	GLuint nSegments;
	GLfloat radius;

	std::vector<Triangle> triangles;
	std::vector<Ring> rings;

	Sphere(void);
	Sphere(GLfloat radius, Vector3 vel, Vector3 pos);
	Sphere(const Sphere& toCopy);
	Sphere& operator= (const Sphere& toCopy);
	~Sphere(void);

	void Init(GLuint program);
	virtual void Update();
	void Bounding();
	void GenerateVertices();
	void GenerateTriangles();
	void SetPoints();
};

#endif