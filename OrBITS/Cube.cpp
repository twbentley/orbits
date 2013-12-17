#include "Cube.h"

Cube::Cube(void) : Shape() { }

// Parameterized constructor
Cube::Cube(GLfloat width, Vector3 vel, Vector3 pos) : Shape(width, vel, pos)
{
	// Set number of vertices and required points for a Cube
	NUM_VERTS = 8;
	NUM_POINTS = 6 * 2 * 3; // faces * triangles per face * points per triangle
}

Cube::Cube(const Cube& toCopy)
{
	if (this != &toCopy)
	{
		this->width = toCopy.width;
		this->vel = toCopy.vel;
		this->pos = toCopy.pos;
		this->aabb.min = toCopy.aabb.min;
		this->aabb.max = toCopy.aabb.max;
		
		for (int i = 0; i < NUM_VERTS; i++)
			vertices[i] = toCopy.vertices[i];
		for (int i = 0; i < NUM_POINTS; i++)
			points[i] = toCopy.points[i];
		// Wait to finish until we have completed new buffer set-up
	}
}

Cube& Cube::operator= (const Cube& toCopy)
{
	if (this != &toCopy)
	{

	}
	return *this;
}

Cube::~Cube(void)
{
	int i = 0;
}

void Cube::Init(GLuint program)
{
	vertices = new Vector3[NUM_VERTS];
	points = new Vector3[NUM_POINTS];
	base_colors = new Vector4[NUM_VERTS];
	colors = new Vector4[NUM_POINTS];

	#pragma region Vertices
	vertices[0] = Vector3(  width,  width, width );
	vertices[1] = Vector3( -width,  width, width );
	vertices[2] = Vector3( -width, -width, width );
	vertices[3] = Vector3(  width, -width, width );
	vertices[4] = Vector3(  width, -width, -width);
	vertices[5] = Vector3(  width,  width, -width);
	vertices[6] = Vector3( -width,  width, -width);
	vertices[7] = Vector3( -width, -width, -width);
	#pragma endregion

	#pragma region Colors
	base_colors[0] = Vector4(  1.0f, 0.0f,  0.0f, .50f );	// red
	base_colors[1] = Vector4(  0.0f, 1.0f,  0.0f, .50f );	// green
	base_colors[2] = Vector4(  0.0f, 0.0f,  1.0f, .50f );	// blue
	base_colors[3] = Vector4(  1.0f, 0.0f,  1.0f, .50f );	// purple
	base_colors[4] = Vector4(  0.0f, 1.0f,  1.0f, .50f );	// turquoise
	base_colors[5] = Vector4(  1.0f, 1.0f,  0.0f, .50f );	// yellow
	#pragma endregion

	// Triangles' vertices
	#pragma region Points
	points[0] = vertices[0];
	points[1] = vertices[1];
	points[2] = vertices[2];
	// front	
	points[3] = vertices[2];
	points[4] = vertices[3];
	points[5] = vertices[0];
               
	points[6] = vertices[0];
	points[7] = vertices[3];
	points[8] = vertices[4];
	//	right		
	points[9]  = vertices[4];
	points[10] = vertices[5]; 
	points[11] = vertices[0]; 
               
	points[12]  = vertices[0];
	points[13]  = vertices[5];
	points[14]  = vertices[6];
	// top
	points[15]  = vertices[6];
	points[16]  = vertices[1];
	points[17]  = vertices[0];
               
	points[18]  = vertices[7];
	points[19]  = vertices[2];
	points[20]  = vertices[1];
	// left
	points[21]  = vertices[1];
	points[22]  = vertices[6];
	points[23]  = vertices[7];
               
	points[24]  = vertices[7];
	points[25]  = vertices[6];
	points[26]  = vertices[5];
	// back
	points[27]  = vertices[5];
	points[28]  = vertices[4];
	points[29]  = vertices[7];
               
	points[30]  = vertices[7];
	points[31]  = vertices[2];
	points[32]  = vertices[3];
	// bottom	
	points[33]  = vertices[3];
	points[34]  = vertices[4];
	points[35]  = vertices[7];
	#pragma endregion

	// Colors associated with triangles' vertices
	#pragma region Colors
	colors[0] = base_colors[0];
	colors[1] = base_colors[0];
	colors[2] = base_colors[0];
	// front	
	colors[3] = base_colors[0];
	colors[4] = base_colors[0];
	colors[5] = base_colors[0];
               
	colors[6] = base_colors[1];
	colors[7] = base_colors[1];
	colors[8] = base_colors[1];
	//	right		
	colors[9]  = base_colors[1];
	colors[10] = base_colors[1]; 
	colors[11] = base_colors[1]; 
               
	colors[12]  = base_colors[2];
	colors[13]  = base_colors[2];
	colors[14]  = base_colors[2];
	// top					  
	colors[15]  = base_colors[2];
	colors[16]  = base_colors[2];
	colors[17]  = base_colors[2];
               
	colors[18]  = base_colors[3];
	colors[19]  = base_colors[3];
	colors[20]  = base_colors[3];
	// left					  
	colors[21]  = base_colors[3];
	colors[22]  = base_colors[3];
	colors[23]  = base_colors[3];
               
	colors[24]  = base_colors[4];
	colors[25]  = base_colors[4];
	colors[26]  = base_colors[4];
	// back					  
	colors[27]  = base_colors[4];
	colors[28]  = base_colors[4];
	colors[29]  = base_colors[4];
               
	colors[30]  = base_colors[5];
	colors[31]  = base_colors[5];
	colors[32]  = base_colors[5];
	// bottom				  
	colors[33]  = base_colors[5];
	colors[34]  = base_colors[5];
	colors[35]  = base_colors[5];
	#pragma endregion

	//ComputeAABB();

	InitOpenGL(program);
}

// Update specific for a cube
void Cube::Update()
{
	// Rotation, Forces and other physics updates
	Matrix4::UpdateRotationMatrix(rotMatrix, 'z', 1.f);
	Matrix4::UpdateRotationMatrix(rotMatrix, 'y', .5f);
	Matrix4::UpdateRotationMatrix(rotMatrix, 'x', .25f);

	// Default update
	Shape::Update();

	// Update rotation and translation for cube
	GLuint vRotateLoc = glGetUniformLocation(myShaderProgram, "vRotate");
	glUniformMatrix4fv(vRotateLoc, 1, GL_TRUE, (GLfloat*)rotMatrix);
	GLuint vTransLoc = glGetUniformLocation(myShaderProgram, "vTrans");
	glUniformMatrix4fv(vTransLoc, 1, GL_TRUE, (GLfloat*)transMatrix);
}