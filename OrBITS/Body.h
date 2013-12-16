#ifndef BODY_H
#define BODY_H

#include "Globals.h"
#include "Sphere.h"
#include "Angel.h"
#include <math.h>
#include <vector>
#include "AABB.h"
#include <string>
#include <queue>
#include <vector>

class Body
{
public:
	Vector3 pos;
	Vector3 vel;
	Vector3 accel;

	float mass;
	float radius;
	std::string type;

	Sphere* sphere;
	std::queue<Vector3> trail;

	std::vector<Vector3> points;
	Vector4* colors;
	Matrix4 transMatrix;
	Matrix4 rotMatrix;

	GLuint myShaderProgram;
	GLuint myBuffer;
	GLuint vao;

	int frameCount;
	int NUM_POINTS; // for trail


	Body(float radius, Vector3 pos, Vector3 vel, std::string type, GLuint myShaderProgram);
	~Body(void);

	void InitOpenGL();
	void Init();
	void SetOrbit(Body& myParent, float radius, Vector3 normal, float rotation, float semiMajLMult);
	void CalcMass();
	void UpdateTrail();
	void RenderTrail();
	void Update();
	void Render();
};

#endif // BODY_H