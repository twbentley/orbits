#include "AABB.h"

// Set up bounding box as a cube at a position
AABB::AABB(void)
{
	NUM_VERTS = 8;
	vertices = nullptr;
	colors = nullptr;
	Matrix4::UpdatePositionMatrix(transMatrix, center.x, center.y, center.z);
}

// Destructor
AABB::~AABB(void)
{
	if (vertices!=nullptr)
		delete[] vertices;
	if (colors!=nullptr)
		delete[] colors;
}

// Initialize bounding box
void AABB::Init(GLuint program)
{
	vertices = new Vector3[NUM_VERTS];
	colors = new Vector4[NUM_VERTS];

	width = 0.5f;

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

	colors[0] = Vector4(  0.0f,0.0f,0.0f, 1.f );	//black
	colors[1] = Vector4(  0.0f,0.0f,0.0f, 1.f );	//black
	colors[2] = Vector4(  0.0f,0.0f,0.0f, 1.f );	//black
	colors[3] = Vector4(  0.0f,0.0f,0.0f, 1.f );	//black
	colors[4] = Vector4(  0.0f,0.0f,0.0f, 1.f );	//black
	colors[5] = Vector4(  0.0f,0.0f,0.0f, 1.f );	//black

	InitOpenGL(program);
}

// Initialize OpenGL for the bonding box
void AABB::InitOpenGL(GLuint program)
{
	myShaderProgram = program;
	// Create a vertex array object
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

	// Create and initialize a buffer object for each circle.
    glGenBuffers( 1, &myBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, myBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(*vertices) * NUM_VERTS + sizeof(*colors) * NUM_VERTS, NULL, GL_STATIC_DRAW );					// Start at offset 0, go for NUM_VERTS
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(*vertices) * NUM_VERTS, vertices );							// Start at offset 0, go for NUM_VERTS
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(*vertices) * NUM_VERTS, sizeof(*colors) * NUM_VERTS, colors );	// Start at offset NUM_VERTS, go for NUM_VERTS

	// Load shaders and use the resulting shader program
    //glUseProgram( myShaderProgram );

	// Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( myShaderProgram, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	GLuint vRotateLoc = glGetUniformLocation(program, "vRotate");
	glUniformMatrix4fv(vRotateLoc, 1, GL_TRUE, (GLfloat*)rotMatrix);

	GLuint vTransLoc = glGetUniformLocation(program, "vTrans");
	glUniformMatrix4fv(vTransLoc, 1, GL_TRUE, (GLfloat*)transMatrix);

	// colors
	GLuint vfColorLoc = glGetAttribLocation(program, "vfColor");
	glEnableVertexAttribArray(vfColorLoc);
	glVertexAttribPointer(vfColorLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(*vertices) * NUM_VERTS));
	glUniform4f(vfColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
}

// Draw bounding box
void AABB::Render()
{
	// Use the buffer and shader for each circle.
	glUseProgram( myShaderProgram );
	glBindBuffer( GL_ARRAY_BUFFER, myBuffer );
	glBindVertexArray(vao);

	GLuint loc = glGetAttribLocation( myShaderProgram, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	GLuint vRotateLoc = glGetUniformLocation(myShaderProgram, "vRotate");
	glUniformMatrix4fv(vRotateLoc, 1, GL_TRUE, (GLfloat*)rotMatrix);

	GLuint vTransLoc = glGetUniformLocation(myShaderProgram, "vTrans");
	glUniformMatrix4fv(vTransLoc, 1, GL_TRUE, (GLfloat*)transMatrix);

	// colors
	GLuint vfColorLoc = glGetAttribLocation(myShaderProgram, "vfColor");
	glEnableVertexAttribArray(vfColorLoc);
	glVertexAttribPointer(vfColorLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(*vertices) * NUM_VERTS));
	glUniform4f(vfColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);

	glDrawArrays(GL_LINE_LOOP, 0, NUM_VERTS);
}

// Update bounding box
void AABB::Update()
{
	glUseProgram( myShaderProgram );
	glBindBuffer( GL_ARRAY_BUFFER, myBuffer );
	glBindVertexArray(vao);

	vertices[0] = min;
	vertices[1] = Vector3(max.x,min.y,min.z);
	vertices[2] = Vector3(max.x,max.y,min.z);
	vertices[3] = max;
	vertices[4] = Vector3(max.x,min.y,max.z);
	vertices[5] = Vector3(min.x,min.y,max.z);
	vertices[6] = Vector3(min.x,max.y,max.z);
	vertices[7] = Vector3(min.x,max.y,min.z);

	// Print vertices for debugging
	for(int i = 0; i < NUM_VERTS; i++)
	{
		//std::cout << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << std::endl;
	}

	// Reload points (they were recalculated)
	glBufferData( GL_ARRAY_BUFFER, sizeof(*vertices) * NUM_VERTS + sizeof(*colors) * NUM_VERTS, NULL, GL_STATIC_DRAW );					// Start at offset 0, go for NUM_VERTS
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(*vertices) * NUM_VERTS, vertices );							// Start at offset 0, go for NUM_VERTS
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(*vertices) * NUM_VERTS, sizeof(*colors) * NUM_VERTS, colors );

	// Set new position
	Matrix4::SetPositionMatrix(transMatrix, center.x, center.y, center.z); // should be pos... i think
	//Matrix4::UpdateRotationMatrix(rotMatrix, 'z', 1.f);

	// Set new rotation
	GLuint vRotateLoc = glGetUniformLocation(myShaderProgram, "vRotate");
	glUniformMatrix4fv(vRotateLoc, 1, GL_TRUE, (GLfloat*)rotMatrix);

	// Set new translation
	GLuint vTransLoc = glGetUniformLocation(myShaderProgram, "vTrans");
	glUniformMatrix4fv(vTransLoc, 1, GL_TRUE, (GLfloat*)transMatrix);
}

// Detect collision between 2 objects
bool AABB::BasicColDetect(AABB& a, AABB& b)
{
	Vector3 const aMin_G = a.center + a.min; // To global
	Vector3 const bMin_G = b.center + b.min; // To global
	Vector3 const aMax_G = a.center + a.max; // To global
	Vector3 const bMax_G = b.center + b.max; // To global

	// X-axis
	if (aMin_G.x <= bMin_G.x) // First is smaller
	{
		if (bMin_G.x <= aMax_G.x){}
			// overlapping on x, continue check
		else
			return false;
	}
	else // second is smaller
	{
		if (aMin_G.x <= bMax_G.x){}
			// overlapping on x, continue check
		else
			return false;
	}

	// Y-axis
	if (aMin_G.y <= bMin_G.y) // First is smaller
	{
		if (bMin_G.y <= aMax_G.y){}
			// overlapping on x and y, so continue checking
		else
			return false;
	}
	else // second is smaller
	{
		if (aMin_G.y <= bMax_G.y){}
			// overlapping on x and y, so continue checking
		else
			return false;
	}

	// Z-axis
	if (aMin_G.z <= bMin_G.z) // First is smaller
	{
		if (bMin_G.z <= aMax_G.z)
		{
			// overlapping on x, y, and z
			return true;
		}
		else
			return false;
	}
	else // second is smaller
	{
		if (aMin_G.z <= bMax_G.z)
		{
			// overlapping on x, y, and z
			return true;
		}
		else
			return false;
	}
	return false;
}