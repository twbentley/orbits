#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"

class Cube : public Shape
{
public:
	Cube(void);
	Cube(GLfloat width, Vector3 vel, Vector3 pos);
	Cube(const Cube& toCopy);
	Cube& operator= (const Cube& toCopy);
	~Cube(void);
	
	void Init(GLuint program);

	virtual void Update();

	void Bounding();
};
#endif // CUBE_H