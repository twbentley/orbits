#ifndef SHAPE_H
#define SHAPE_H

#include "Angel.h"
#include <math.h>
#include <vector>
#include "AABB.h"

class Shape
{
public:	
	struct Triangle
	{
		Vector3 point1;
		Vector3 point2;
		Vector3 point3;
	};

	Vector3* vertices;
	Vector4* base_colors;
	Vector3* points;
	Vector4* colors;

	Matrix4 transMatrix;
	Matrix4 rotMatrix;
	Matrix4 scaleMatrix;
	Matrix4 skewMatrix;

	GLfloat width;
	Vector3 vel;
	Vector3 pos;
	AABB aabb;
	
	GLuint myShaderProgram;
	GLuint myBuffer;
	GLuint vao;

	int NUM_POINTS;
	int NUM_VERTS;

	Shape(void);
	Shape(GLfloat width, Vector3 vel, Vector3 pos);
	Shape(const Shape& toCopy);
	Shape& operator= (const Shape& toCopy);
	~Shape(void);

	void InitOpenGL(GLuint program);
	virtual void Render();
	virtual void Update();
	void Bounding();
	void ComputeAABB();
};

#endif