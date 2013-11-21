#include "Shape.h"

Shape::Shape(void)
{
	width = .5;
	vel = Vector3(0,0,0);
	pos = Vector3(0,0,0);
}

// Parameterized constructor
Shape::Shape(GLfloat width, Vector3 vel, Vector3 pos)
{
	// Set width, velocity, position, and position
	this->width = width;
	this->vel = vel;
	this->pos = pos;
	Matrix4::UpdatePositionMatrix(transMatrix, pos.x, pos.y, pos.z);
}

Shape::Shape(const Shape& toCopy)
{
}

Shape& Shape::operator= (const Shape& toCopy)
{
	return *this;
}

// Destructor
Shape::~Shape(void)
{
	if (vertices!=nullptr)
		delete[] vertices;
	if (base_colors!=nullptr)
		delete[] base_colors;
	if (points!=nullptr)
		delete[] points;
	if (colors!=nullptr)
		delete[] colors;
}

// Initialize OpenGL for this shape
void Shape::InitOpenGL(GLuint program)
{
	// Get the shader program
	myShaderProgram = program;

	// Create a vertex array object
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

	// Create and initialize a buffer object for each circle.
    glGenBuffers( 1, &myBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, myBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(*points) * NUM_POINTS + sizeof(*colors) * NUM_POINTS, NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(*points) * NUM_POINTS, points );							// Start at offset 0, go for NUM_VERTS
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(*points) * NUM_POINTS, sizeof(*colors) * NUM_POINTS, colors );	// Start at offset NUM_VERTS, go for NUM_VERTS

	// Load shaders and use the resulting shader program
    //glUseProgram( myShaderProgram );

	// Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( myShaderProgram, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	// Set value of rotation for this object
	GLuint vRotateLoc = glGetUniformLocation(myShaderProgram, "vRotate");
	glUniformMatrix4fv(vRotateLoc, 1, GL_TRUE, (GLfloat*)rotMatrix);

	// Set value of translation for this object
	GLuint vTransLoc = glGetUniformLocation(myShaderProgram, "vTrans");
	glUniformMatrix4fv(vTransLoc, 1, GL_TRUE, (GLfloat*)transMatrix);

	// Set up colors for this object
	GLuint vfColorLoc = glGetAttribLocation(myShaderProgram, "vfColor");
	glEnableVertexAttribArray(vfColorLoc);
	glVertexAttribPointer(vfColorLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(*points) * NUM_POINTS));
	glUniform4f(vfColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);

	// Initialize bounding for this object
	aabb.Init(program);
}

// Draw object to screen
void Shape::Render()
{
	// Use the vertex and array buffers and shader for each object
	glUseProgram( myShaderProgram );
	glBindBuffer( GL_ARRAY_BUFFER, myBuffer );
	glBindVertexArray(vao);

	// Draw points
	glDrawArrays(GL_TRIANGLES, 0, NUM_POINTS);

	// Draw bounding box
	aabb.Render();
}

// Update shape
void Shape::Update()
{
	// Use correct shader program and buffers
	glUseProgram( myShaderProgram );
	glBindBuffer( GL_ARRAY_BUFFER, myBuffer );
	glBindVertexArray(vao);

	// Rotation, Forces and other physics updates
	/*Matrix4::UpdateRotationMatrix(rotMatrix, 'z', 1.f);
	Matrix4::UpdateRotationMatrix(rotMatrix, 'y', .5f);*/
	
	// Euler integration for position, acceleration currently not implemented
	//vel += accel;
	pos += vel;
	/*if (vel.length(vel) < .005f)
	{
		vel = Vector3::normalize(vel);
		vel*= 0.005f;
	}*/

	// Bound and check collisions
	ComputeAABB();
	Bounding();

	// Move object correctly
	Matrix4::SetPositionMatrix(transMatrix, pos.x, pos.y, pos.z);
}

// Compute the bounding box for this shape
void Shape::ComputeAABB()
{
	// Get rotated vertices
	Vector4* tempVerts = new Vector4[NUM_VERTS];
	for(int i = 0; i < NUM_VERTS; i++)
	{
		tempVerts[i] = Vector4(vertices[i].x, vertices[i].y, vertices[i].z, 1.0f);
		tempVerts[i] = rotMatrix * tempVerts[i];
	}

	// These are in local coordinates, relative shape's origin
	Vector3 newMin = Vector3(10,10,10);		// larger than possible
	Vector3 newMax = Vector3(-10,-10,-10);	// smaller than possible

	// Get min, max, and center for bounding box
	for (int i = 0; i < NUM_VERTS; i++) // Change to length of vertices[]
	{
		if (tempVerts[i].x < newMin.x)
			newMin.x = tempVerts[i].x;
		if (tempVerts[i].y < newMin.y)
			newMin.y = tempVerts[i].y;
		if (tempVerts[i].z < newMin.z)
			newMin.z = tempVerts[i].z;

		if (tempVerts[i].x > newMax.x)
			newMax.x = tempVerts[i].x;
		if (tempVerts[i].y > newMax.y)
			newMax.y = tempVerts[i].y;
		if (tempVerts[i].z > newMax.z)
			newMax.z = tempVerts[i].z;
	}

	delete [] tempVerts;

	// Set min, max and center for bounding box
	aabb.min = newMin;
	aabb.max = newMax;
	//aabb.center = (aabb.min + aabb.max) * 0.5f + pos;
	aabb.center = pos;

	// Now update the bounding box
	aabb.Update();
}

// Check and respond to world bounding
void Shape::Bounding()
{
	if (pos.x + aabb.min.x < -1) // Where -1 represents min x of world
	{
		GLfloat distOff = aabb.min.x - (-1 - pos.x);
		pos.x -= distOff;
		if(vel.x < 0)
			vel.x *= -1;
	}
	else if (pos.x + aabb.max.x > 1) // Where 1 represents max x of world
	{
		GLfloat distOff = aabb.max.x - (1.f - pos.x);
		pos.x -= distOff;
		if(vel.x > 0)
			vel.x *= -1;
	}
	if (pos.y + aabb.min.y < -1) // Where -1 represents min y of world
	{
		GLfloat distOff = aabb.min.y - (-1 - pos.y);
		pos.y -= distOff;
		if(vel.y < 0)
			vel.y *= -1;
	}
	else if (pos.y + aabb.max.y > 1) // Where 1 represents max y of world
	{
		GLfloat distOff = aabb.max.y - (1.f - pos.y);
		pos.y -= distOff;
		if(vel.y > 0)
			vel.y *= -1;
	}
	if (pos.z + aabb.min.z < -1) // Where -1 represents min z of world
	{
		GLfloat distOff = aabb.min.z - (-1 - pos.z);
		pos.z -= distOff;
		if(vel.z < 0)
			vel.z *= -1;
	}
	else if (pos.z + aabb.max.z > 1) // Where 1 represents max z of world
	{
		GLfloat distOff = aabb.max.z - (1.f - pos.z);
		pos.z -= distOff;
		if(vel.z > 0)
			vel.z *= -1;
	}
}