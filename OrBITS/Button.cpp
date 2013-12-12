#include "Button.h"


Button::Button(GLfloat width, GLfloat depth)
{
	NUM_VERTS = 4;
	this->width = width;
	this->depth = depth;
}


Button::~Button(void)
{
}

void Button::Init(GLuint program)
{
	vertices = new Vector3[NUM_VERTS];
	UVs = new Vector3[NUM_VERTS];
	colors = new Vector4[NUM_VERTS];

	vertices[0] = Vector3( -width	, width		, depth );
	vertices[1] = Vector3( width	, width		, depth );
	vertices[2] = Vector3( width	, -width	, depth );
	vertices[3] = Vector3( -width	, -width	, depth );

	UVs[0] = Vector3( 0.0f, 0.0f, 0 );
	UVs[1] = Vector3( 1.0f, 0.0f, 0 );
	UVs[2] = Vector3( 1.0f, 1.0f, 0 );
	UVs[3] = Vector3( 0.0f, 1.0f, 0 );

	colors[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	colors[1] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	colors[2] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	colors[3] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

	InitOpenGL(program);
}

void Button::InitOpenGL(GLuint program)
{
	// Get the shader program
	myShaderProgram = program;

	// Create a vertex array object
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

	// Create and initialize a buffer object for each circle.
    glGenBuffers( 1, &myBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, myBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(*vertices) * NUM_VERTS + sizeof(*colors) * NUM_VERTS + sizeof(*UVs) * NUM_VERTS, NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(*vertices) * NUM_VERTS, vertices );							// Start at offset 0, go for NUM_VERTS
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(*vertices) * NUM_VERTS, sizeof(*colors) * NUM_VERTS, colors );	// Start at offset NUM_VERTS, go for NUM_VERTS
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(*vertices) * NUM_VERTS + sizeof(*colors) * NUM_VERTS, sizeof(*UVs) * NUM_VERTS, UVs );

	// Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( myShaderProgram, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	// Initialize the UV attribute from the vertex shader
	GLuint UVloc = glGetAttribLocation( myShaderProgram, "vertexUV" );
    glEnableVertexAttribArray( UVloc );
    glVertexAttribPointer( UVloc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	// Set value of rotation for this object
	GLuint vRotateLoc = glGetUniformLocation(myShaderProgram, "vRotate");
	glUniformMatrix4fv(vRotateLoc, 1, GL_TRUE, (GLfloat*)rotMatrix);

	// Set value of translation for this object
	GLuint vTransLoc = glGetUniformLocation(myShaderProgram, "vTrans");
	glUniformMatrix4fv(vTransLoc, 1, GL_TRUE, (GLfloat*)transMatrix);

	// Set up colors for this object
	GLuint vfColorLoc = glGetAttribLocation(myShaderProgram, "vfColor");
	glEnableVertexAttribArray(vfColorLoc);
	glVertexAttribPointer(vfColorLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(*vertices) * NUM_VERTS));
	glUniform4f(vfColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);

	// TODO: Load Texture
	FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType("./blank.bmp", 0), "./blank.bmp");
	FIBITMAP *pImage = FreeImage_ConvertTo32Bits(bitmap);
	int nWidth = FreeImage_GetWidth(pImage);
	int nHeight = FreeImage_GetHeight(pImage);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void Button::Render()
{
	// Use the vertex and array buffers and shader for each object
	glUseProgram( myShaderProgram );
	glBindBuffer( GL_ARRAY_BUFFER, myBuffer );
	glBindVertexArray(vao);

	glBindTexture(GL_TEXTURE_2D, textureID);

	// Draw points
	glDrawArrays(GL_QUADS, 0, NUM_VERTS);
}

void Button::Update()
{
	// Rotation, Forces and other physics updates
	Matrix4::UpdateRotationMatrix(rotMatrix, 'z', 1.f);
	Matrix4::UpdateRotationMatrix(rotMatrix, 'y', .5f);
	Matrix4::UpdateRotationMatrix(rotMatrix, 'x', .25f);

	// Use correct shader program and buffers
	glUseProgram( myShaderProgram );
	glBindBuffer( GL_ARRAY_BUFFER, myBuffer );
	glBindVertexArray(vao);

	glBindTexture(GL_TEXTURE_2D, textureID);

	//Matrix4::SetPositionMatrix(transMatrix, pos.x, pos.y, pos.z);

	// Update rotation and translation for cube
	GLuint vRotateLoc = glGetUniformLocation(myShaderProgram, "vRotate");
	glUniformMatrix4fv(vRotateLoc, 1, GL_TRUE, (GLfloat*)rotMatrix);
	GLuint vTransLoc = glGetUniformLocation(myShaderProgram, "vTrans");
	glUniformMatrix4fv(vTransLoc, 1, GL_TRUE, (GLfloat*)transMatrix);
}

// TODO: take texture code and put in this class