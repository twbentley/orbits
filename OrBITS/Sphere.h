#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

class Sphere :
	public Shape
{
public:
	GLuint nRings;
	GLuint nSegments;
	GLfloat radius;

	Sphere(void);
	Sphere(GLfloat radius, Vector3 vel, Vector3 pos);
	Sphere(const Sphere& toCopy);
	Sphere& operator= (const Sphere& toCopy);
	~Sphere(void);

	void Init(GLuint program);
	virtual void Update();
	void Bounding();
	void TryCircle();
};

#endif