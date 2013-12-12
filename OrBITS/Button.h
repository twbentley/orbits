#pragma once
#include "Shape.h"
#include "vec.h"

#include <GL/FreeImage.h>
#include <GL/FreeImage.h>

class Button
{
public:
	Vector3* vertices;
	vec2* UVs;
	Vector4* colors;
	Matrix4 transMatrix;
	Matrix4 rotMatrix;

	GLfloat width;
	GLfloat depth;
	
	GLuint myShaderProgram;
	GLuint myBuffer;
	GLuint vao;
	GLuint textureID;

	int NUM_VERTS;

	Button(GLfloat width, GLfloat depth);
	~Button(void);

	void Init(GLuint program);
	void InitOpenGL(GLuint program);

	void Render();
	void Update();
};

