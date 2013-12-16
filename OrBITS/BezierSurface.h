#ifndef BEZIER_SURFACE_H
#define BEZIER_SURFACE_H

#include "GL/glew.h"/*
#include "Globals.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "MathFunctions.h"*/
#include "Angel.h"
#include <math.h>
#include <vector>

class BezierSurface
{
private:
	std::vector<Vector3> _controlPoints1;							// The Control Points1 that calculates the Surface
	std::vector<Vector3> _controlPoints2;							// The Control Points2 that calculates the Surface

	std::vector<std::vector<Vector3>> _pointsOnTheSurface;		// Points that are a part of the Surface

	std::vector<Vector3> _vertices;								// Verticies and Color of Surface
	std::vector<Vector4> _colors;

	//Matrix4 _model;												// Model Matrix
	GLuint degrees;
	Matrix4 transMatrix;
	Matrix4 rotMatrix;
	Matrix4 scaleMatrix;
	Matrix4 skewMatrix;

	GLuint _curveVertexbuffer;										// Buffers, Note for example Control Points 1 and 2 can be different sizes
	GLuint _controlVertexbuffer1;
	GLuint _controlVertexbuffer2;
	GLuint vao;

	Vector4 _bottomLeftColor;										// Colors of the Surface
	Vector4 _bottomRightColor;
	Vector4 _topLeftColor;
	Vector4 _topRightColor;

	unsigned __int8 _numberOfTimeStamps;							// Number of timesteps, Note could have had two for each 'curve'

	static const Vector4 _controlPointsColor1;					// Static colors to display control points and lines
	static const Vector4 _controlLinesColor1;
	static const Vector4 _controlPointsColor2;
	static const Vector4 _controlLinesColor2;

	std::vector<Vector4>  _colorControlPoints1;					// Colors of the Control Points1
	std::vector<Vector4>  _colorControlLines1;

	std::vector<Vector4>  _colorControlPoints2;					// Colors of the Control Points2
	std::vector<Vector4>  _colorControlLines2;

public:
	GLuint myShaderProgram;

	// Constructor
	BezierSurface(GLuint myShaderProgram, std::vector<Vector3> points1, std::vector<Vector3> points2, unsigned __int8 numberOfTimeStamps = 20, 
		Vector4 bottomLeftColor = Vector4( 0.0f, 0.0f, 1.0f, 1.0f ), Vector4 bottomRightColor = Vector4( 1.0f, 1.0f, 1.0f, 1.0f ), 
		Vector4 topLeftColor = Vector4( 1.0f, 0.0f, 0.0f, 1.0f ), Vector4 topRightColor = Vector4( 0.0f, 1.0f, 0.0f, 1.0f ));

	// Deconstructor
	~BezierSurface(void);

	// Create points on the Curve
	void createPoints();

	void fillBuffer();

	void fillColors();

	// Get the Color based on current location of curve
	// Influenced on corner colors
	Vector4 getColor( int widthIndex, int heightIndex, int widthSize, int heightSize );

	// Updates the curve
	void Update();
	// Displays the Curve
	void Display();

	//Initialize vertices
	void Init(GLuint program);

	// Properties 
	std::vector<Vector3>* controlPoints1(){ return &_controlPoints1; }		// Get address of control Points
	std::vector<Vector3>* controlPoints2(){ return &_controlPoints2; }

	// Non-class BezierSurface Methods
	static int factorial(const int num); // TODO: Construct global class and move to
};

#endif

