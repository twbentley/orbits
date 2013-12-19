#pragma once
#include "Shape.h"

#include <GL/FreeImage.h>
#include <GL/FreeImage.h>

class Button
{
public:
	Vector3* vertices;
	Vector3* UVs;
	Matrix4 transMatrix;
	Matrix4 rotMatrix;

	GLfloat width;
	GLfloat depth;
	std::string imagePath;
	
	GLuint myShaderProgram;
	GLuint myBuffer;
	GLuint vao;
	GLuint textureID;

	int NUM_VERTS;

	Button(GLfloat width, GLfloat depth, std::string imageName);
	~Button(void);

	void Init(GLuint program);
	void InitOpenGL(GLuint program);

	void Render();
	void Update();

	void Click(void);
};

