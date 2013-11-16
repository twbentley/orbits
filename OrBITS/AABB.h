#ifndef AABB_H
#define AABB_H

#include "Angel.h"

class AABB
{
public:
	Vector3 min;
	Vector3 max;
	Vector3 center;

	Vector3* vertices;
	Vector4* colors;
	Matrix4 rotMatrix;
	Matrix4 transMatrix;

	GLfloat width;
	GLuint myShaderProgram;
	GLuint myBuffer;
	GLuint vao;

	int NUM_VERTS;

	void Init(GLuint program);
	void InitOpenGL(GLuint program);
	virtual void Render();
	virtual void Update();

	AABB(void);
	AABB(GLfloat width, Vector3 vel, Vector3 pos);
	~AABB(void);

	static bool BasicColDetect(AABB& first, AABB& second);
};

#endif // AABB_H
