#include "Body.h"
using namespace std;

Body::Body(float radius, Vector3 pos, Vector3 vel, string type, GLuint myShaderProgram)
{
	this->radius = radius;
	this->pos = pos;
	this->vel = vel;
	this->type = type;
	this->myShaderProgram = myShaderProgram;
	CalcMass();
	accel = Vector3(0,0,0);

	if(this->type == SUN)
		sphere = new Sphere(this->radius, Vector3(0,0,0), this->pos, 50); // no vel, just visual representation
	if(this->type == PLANET)
		sphere = new Sphere(this->radius, Vector3(0,0,0), this->pos, 25); // no vel, just visual representation
	if(this->type == ASTEROID)
		sphere = new Sphere(this->radius, Vector3(0,0,0), this->pos, 5); // no vel, just visual representation

	sphere->Init(myShaderProgram);
	trail = queue<Vector3>();
	frameCount = 0;
}

Body::~Body(void)
{
	delete sphere;
}

void Body::Init()
{
	NUM_POINTS = 100;

	for (int i = 0; i < NUM_POINTS; i++)
	{
		trail.push(pos);
	}

	points = vector<Vector3>();//[NUM_POINTS];
	for (int i = 0; i < NUM_POINTS; i++)
	{
		points.push_back(pos);
	}

	colors = new Vector4[NUM_POINTS];
	for (int i = 0; i < NUM_POINTS; i++)
	{
		colors[i] = Vector4(0,0,0,1);
	}

	InitOpenGL();
}

void Body::InitOpenGL()
{
	// Create a vertex array object
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

	// Create and initialize a buffer object for each circle.
    glGenBuffers( 1, &myBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, myBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(points[0]) * NUM_POINTS + sizeof(*colors) * NUM_POINTS, NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points[0]) * NUM_POINTS, &points[0] );							// Start at offset 0, go for NUM_VERTS
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points[0]) * NUM_POINTS, sizeof(*colors) * NUM_POINTS, colors );	// Start at offset NUM_VERTS, go for NUM_VERTS

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

	// Set value of translation for this object
	GLuint vScaleLoc = glGetUniformLocation(myShaderProgram, "vScale");
	glUniformMatrix4fv(vScaleLoc, 1, GL_TRUE, (GLfloat*)scaleMatrix);

	GLuint vSkewLoc = glGetUniformLocation(myShaderProgram, "vSkew");
	glUniformMatrix4fv(vSkewLoc, 1, GL_TRUE, (GLfloat*)skewMatrix);

	// Set up colors for this object
	GLuint vfColorLoc = glGetAttribLocation(myShaderProgram, "vfColor");
	glEnableVertexAttribArray(vfColorLoc);
	glVertexAttribPointer(vfColorLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points[0]) * NUM_POINTS));
	glUniform4f(vfColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
}

void Body::Update()
{
	++frameCount;
	if (frameCount % 3 == 0)
	{
		frameCount = 0;
	}
	//frameCount = frameCount % 10; // every 10 frames, resets frameCount

	if(type != SUN) //Sun isnt affected by gravity/doesnt move
	{
		vel += (accel);
		pos += (vel);

		accel = Vector3(0, 0, 0); // Reset for next update
    }
	sphere->pos = this->pos;
	sphere->Update();
	UpdateTrail();
}

void Body::UpdateTrail()
{
	if (frameCount == 0)
	{
		trail.push(pos);
		points.insert(points.begin(), pos);
	}

	if (trail.size() > NUM_POINTS)
	{
		trail.pop();
		points.pop_back();
	}

	/*queue<Vector3> temp = queue<Vector3>(trail);
	for (int i = 0; i < NUM_POINTS; i++)
	{
		points[i] = Vector3(temp.front());
		temp.pop();
	}*/
}

void Body::RenderTrail()
{
	// Use the buffer and shader for each circle.
	glUseProgram( myShaderProgram );
	glBindBuffer( GL_ARRAY_BUFFER, myBuffer );
	glBindVertexArray(vao);

	// Reload points (they were recalculated)
	glBufferData( GL_ARRAY_BUFFER, sizeof(points[0]) * NUM_POINTS + sizeof(*colors) * NUM_POINTS, NULL, GL_STATIC_DRAW );					// Start at offset 0, go for NUM_VERTS
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points[0]) * NUM_POINTS, &points[0] );							// Start at offset 0, go for NUM_VERTS
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points[0]) * NUM_POINTS, sizeof(*colors) * NUM_POINTS, colors );

	GLuint loc = glGetAttribLocation( myShaderProgram, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	GLuint vRotateLoc = glGetUniformLocation(myShaderProgram, "vRotate");
	glUniformMatrix4fv(vRotateLoc, 1, GL_TRUE, (GLfloat*)rotMatrix);

	GLuint vTransLoc = glGetUniformLocation(myShaderProgram, "vTrans");
	glUniformMatrix4fv(vTransLoc, 1, GL_TRUE, (GLfloat*)transMatrix);

	GLuint vScaleLoc = glGetUniformLocation(myShaderProgram, "vScale");
	glUniformMatrix4fv(vScaleLoc, 1, GL_TRUE, (GLfloat*)scaleMatrix);

	GLuint vSkewLoc = glGetUniformLocation(myShaderProgram, "vSkew");
	glUniformMatrix4fv(vSkewLoc, 1, GL_TRUE, (GLfloat*)skewMatrix);

	// colors
	GLuint vfColorLoc = glGetAttribLocation(myShaderProgram, "vfColor");
	glEnableVertexAttribArray(vfColorLoc);
	glVertexAttribPointer(vfColorLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points[0]) * NUM_POINTS));
	glUniform4f(vfColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);

	glDrawArrays(GL_LINE_STRIP, 0, NUM_POINTS);
}

void Body::Render()
{
	sphere->Render();
	RenderTrail();
}

void Body::CalcMass()
{
	float massScalar;
	if (type == SUN)
		massScalar = 1;
	else if (type == PLANET)
		massScalar = .1f;
	else if (type == ASTEROID)
		massScalar = .01f;
	else
		massScalar = 999999;

	mass = static_cast<float>(massScalar*(((4.f/3.f)*PI*(radius*radius*radius))/10.f));
}

void Body::SetOrbit(Body& myParent, float radius, Vector3 normal, float rotation, float semiMajLMult) // normal is axis of rotation, should also add pos or neg velocity
  {
    float unknownY = -normal.x/normal.y; // 0 = x1*x2 + y1*unknownY + z1*z2 (dot product)
                                         // 0 = x1 + y1*unknownY
                                         // unknownY = -x1/y1
    if(normal.y == 0)
    {unknownY = 0;}
    Vector3 alongX = Vector3(1,unknownY,0); // is orthoganal to normal
    Vector3 tempPos;
	
    if(Vector3::angleBetween(normal,alongX) != 0)
    {
		tempPos = Vector3::cross(normal, alongX);     // pos from parent
    }
    else
    {
      tempPos = Vector3(0,1,0);
    }
	tempPos = Vector3::normalize(tempPos);
    tempPos *= (radius);                // scale to appropriate dist out
    // rotate about normal, use 3D Rotation Matrix
    pos = myParent.pos + tempPos;   // place in world!
    //println("tempPos.mag: "+tempPos.mag()+", tempPos: " +tempPos+ ", actPos: " + pos);
    if(semiMajLMult == 0)
    {
      semiMajLMult = .00001f;
    }
    float semiMajL = radius*semiMajLMult; //elliptical, where (dist from sun(foci) to planet at 0 degrees)*4 = semi-minor axis
    //semiMajL = radius; //circular
    //semiMajL = radius*99999999; //parabolic
    //semiMajL = radius*-2f; // for hyperbola, except will start at y-int 
    
    float velMag = (float)(sqrt((myParent.mass*myParent.mass*G) * ((2/((myParent.mass + mass)*radius)) - (1/((myParent.mass+mass)*semiMajL))) )); // For any orbit
	Vector3 velDir = Vector3::cross(normal, tempPos);  // orthoganal to pos from origin and normal from origin
    velDir = velDir.normalize(velDir);
    velDir *= velMag;  // scale to calc'd value
    vel = velDir;
    vel += myParent.vel;
  }