#include "Cube.h"
#include "Sphere.h"
#include "BezierSurface.h"
#include "Camera.h"
#include "Button.h"
#include "Globals.h"
#include "Body.h"

#include <vector>
#include <random>
#include <time.h>

// Define for leak detection
#define _CRTDBG_MAP_ALLOC

// Prevent console window from opening
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

enum STATE { MENU, PLAY, PAUSE };

// GLFW window
GLFWwindow* window;
// Shader program
GLuint program;
// Shader program for buttons/menu
GLuint button_program;
// Shapes in world
Shape** shapes;
// Celestial Bodies
std::vector<Body*> bodies;
int NUM_BODIES;
BezierSurface* bezier;

Button* startButton;
Button* pauseImage;
Button* resetButton;
Button* asteroidButton;

// Number of objects in world
int NUM_OBJECTS = 2;

Camera cam;
float prevSeconds;

GLuint prevMouseState;
GLuint currMouseState;
GLuint prevPauseState;
GLuint currPauseState;
STATE gameState = MENU;

unsigned char * data;
GLuint textureID;
double cursorX;
double cursorY;

// Forward initialization
void Initialize();
void CreateShape();
BezierSurface* CreateBezierSurf(); // WOOOAH, TOTALLY AWESOME DUDE!
void Display();
void Input();
void cameraInputCheck();
void TryCircle();
void ResolveCol(Shape& a, Shape&b);
void GenSystem();       // Inits solar system
void GenMoonDemo();     // Inits Sun, planet, moon
void GenTwoBody();		// Inits two planets
void CalcGravity();		// Updates gravitation on planets
void AsteroidAttack();	// Spawn number of asteroids surrounding Sun of set radius
void PlanetCollRes(Body& a, Body& b);

int main(int argc, char **argv)
{	
	// Set up glfw and display window
	if(glfwInit() == 0) return 1;
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "orBITs", NULL, NULL);
	if(window == nullptr) return 1;
	glfwMakeContextCurrent(window);

	// Set up glew
	glewExperimental = GL_TRUE;
	glewInit();
	
	Initialize();

	// GLFW "game loop"
	while(!glfwWindowShouldClose(window))
	{
		Input();
		Display();
		glfwPollEvents();
	}

	// Delete objects;
	for(int i = 0; i < NUM_OBJECTS; i++)
	{
		delete shapes[i];
	}
	// Delete array
	delete [] shapes;
		
	// Close and clean up glfw window
	glfwDestroyWindow(window);
	glfwTerminate();

	// Get memory leaks
	_CrtDumpMemoryLeaks();
}

// Initialize world
void Initialize()
{
	srand(time(NULL));
	// Set world size in 3 dimensions
	WORLD_SIZE = Vector3(100.f, 100.0f, 100.f);

	// Set up depth in OpenGL
	//glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Set the initial camera position
	cam.SetInitialPosition(0.0f, 0.0f, 20.0f);
	cam.SetViewportSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	// Get current time
	prevSeconds = glfwGetTime();

	// BUTTON
	button_program = InitShader("texvshader.glsl", "texfshader.glsl");

	startButton = new Button(0.5f, 0.0f, "button_start");
	startButton->Init(button_program);
	pauseImage = new Button(0.5f, 0.0f, "paused_new");
	Matrix4::SetPositionMatrix(pauseImage->transMatrix, 0.0f, 1.285f, 0.0f);
	pauseImage->Init(button_program);
	resetButton = new Button(0.5f, 0.0f, "button_reset");
	Matrix4::SetPositionMatrix(resetButton->transMatrix, 0.5f, -1.285f, 0.0f);
	resetButton->Init(button_program);
	asteroidButton = new Button(0.5f, 0.0f, "button_asteroid");
	Matrix4::SetPositionMatrix(asteroidButton->transMatrix, 0.5f, -1.285f, 0.0f);
	asteroidButton->Init(button_program);
	// BUTTON

	// Load shaders and use resulting shader program
	program = InitShader("vshader.glsl", "fshader.glsl");

	// Set up view and project matrices in shader
	GLuint viewMat_loc = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(viewMat_loc, 1, GL_FALSE, cam.ViewMatrix());
	GLuint projMat_loc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projMat_loc, 1, GL_FALSE, cam.ProjectionMatrix());
	
	Cube* cube = new Cube( 0.15f, Vector3(0.0f, 0.f, 0.05f), Vector3(8.0f, 0.7f, -35.0f));
	cube->Init(program);
	Sphere* sphere = new Sphere(5.0f, Vector3(0.0f, 0.f, 0.f), Vector3(0.f, 0.f, 0.f), 50);
	sphere->Init(program);

	bezier = CreateBezierSurf();

	shapes = new Shape*[NUM_OBJECTS];
	shapes[0] = cube;
	shapes[1] = sphere;
	//GenSystem();
    //GenMoonDemo();
	GenTwoBody();
	//AsteroidAttack();

    // Initialize the vertex position attribute from the vertex shader
    //GLuint loc = glGetAttribLocation( program, "vPosition" );
    //glEnableVertexAttribArray( loc );
    //glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	// Enable transparency
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set up background color
	//glClearColor(0.1f, 0.1f, 0.1f, 0.01f);
	glClearColor(1.0,1.0,1.0,1.0);
}

// Display objects
void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(gameState == MENU)
	{
		startButton->Update();
		startButton->Render();
	}
	else if(gameState == PAUSE)
	{
		pauseImage->Update();
		pauseImage->Render();

		resetButton->Update();
		resetButton->Render();
	}
	else if(gameState == PLAY)
	{
		bezier->Update();

		CalcGravity();

		asteroidButton->Update();
		asteroidButton->Render();
	}

	if(gameState == PLAY || gameState == PAUSE)
	{
		// Update and render all objects
		for (int i = 0; i <  NUM_OBJECTS; i++)
		{
			if(gameState == PLAY)
				shapes[i]->Update();

			//shapes[i]->Render();
		}

		bezier->Display();

        for (int i = 0; i < NUM_BODIES; i++)
        {
			if(gameState == PLAY)
			{
                bodies[i]->Update();
			}

            bodies[i]->Render();
        }

		//// Resolve conflicts between all objects
		//for(int i = 0; i < NUM_OBJECTS; i++)
		//{
		//	for(int j = i + 1; j < NUM_OBJECTS; j++)
		//	{
		//		ResolveCol(*shapes[i], *shapes[j]);
		//	}
		//}
	}

	glfwSwapBuffers(window);
}

// Handle keyboard and mouse input
void Input()
{
	prevMouseState = currMouseState;
	currMouseState = glfwGetMouseButton(window, 0);
	prevPauseState = currPauseState;
	currPauseState = glfwGetKey(window, GLFW_KEY_P);
	
	if(glfwGetKey(window, GLFW_KEY_ESCAPE)) // Quit Game
		glfwSetWindowShouldClose(window, true);

	if(gameState == PLAY || gameState == PAUSE)
	{
		cameraInputCheck();

		if(currPauseState && !prevPauseState)
			gameState = (gameState == PLAY) ? PAUSE : PLAY;
	}

	if(prevMouseState == GLFW_PRESS && currMouseState == GLFW_RELEASE)
	{
		// Get cursor position (0,0 is upper left hand corner)
		glfwGetCursorPos(window, &cursorX, &cursorY);

		// Convert cursor position to openGL coordinates
		cursorX = (cursorX / SCREEN_WIDTH) * 2.0 - 1.0;
		cursorY = (cursorY / SCREEN_HEIGHT);
		cursorY = (cursorY * 2.0 - 1.0) * -1.0;
		
		// If button
		if(gameState == MENU && (cursorX < startButton->vertices[1].x && cursorX > startButton->vertices[3].x) && (cursorY < startButton->vertices[1].y && cursorY > startButton->vertices[3].x) )
		{
			gameState = PLAY;
		}
		else if(gameState == PAUSE && (cursorX < resetButton->vertices[1].x + resetButton->transMatrix[0][3] && cursorX > resetButton->vertices[3].x + resetButton->transMatrix[0][3])
			&& (cursorY < resetButton->vertices[1].y + resetButton->transMatrix[1][3] && cursorY > resetButton->vertices[3].y + + resetButton->transMatrix[1][3]) )
		{
			cam.Reset();
			cam.position = Vector3(0,0,0);
			Initialize();
		}
		else if( gameState == PLAY && (cursorX < asteroidButton->vertices[1].x + asteroidButton->transMatrix[0][3] && cursorX > asteroidButton->vertices[3].x + asteroidButton->transMatrix[0][3])
			&& (cursorY < asteroidButton->vertices[1].y + asteroidButton->transMatrix[1][3] && cursorY > asteroidButton->vertices[3].y + + asteroidButton->transMatrix[1][3]) )
		{
			AsteroidAttack();
		}
	}
}

void cameraInputCheck()
{
	glUseProgram(program);

	float currentSeconds = glfwGetTime();
	float elapsed_seconds = currentSeconds - prevSeconds;
	prevSeconds = currentSeconds;

	Vector3 right(1.0f, 0.0f, 0.0f);
	Vector3 up(0.0f, 1.0f, 0.0f);
	Vector3 forward(0.0f, 0.0f, 1.0f);

	// control keys
	bool cam_moved = false;
	if (glfwGetKey (window, 'A')) {
		cam.Translate(cam.Right(), -cam.move_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, 'D')) {
		cam.Translate(cam.Right(), cam.move_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_PAGE_UP)) {
		cam.Translate(cam.Up(), cam.move_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_PAGE_DOWN)) {
		cam.Translate(cam.Up(), -cam.move_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, 'W')) {
		cam.Translate(cam.Forward(), -cam.move_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, 'S')) {
		cam.Translate(cam.Forward(), cam.move_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_UP)) {
		cam.Pitch(-cam.rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_DOWN)) {
		cam.Pitch(cam.rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_LEFT)) {
		cam.Yaw(-cam.rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_RIGHT)) {
		cam.Yaw(cam.rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, 'Z')) {
		cam.Roll(cam.rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, 'X')) {
		cam.Roll(-cam.rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, ' ')) {
		cam.Reset();
		cam.Translate(0.0f, 0.0f, 2.0f);
		cam_moved = true;
	}

	// update view matrix
	if (cam_moved) 
	{
		GLuint viewMat_loc = glGetUniformLocation(program, "view");
		glUniformMatrix4fv(viewMat_loc, 1, GL_FALSE, cam.ViewMatrix());
	}

	bool proj_changed = false;
	if (glfwGetKey (window, GLFW_KEY_MINUS)) {
		cam.Zoom(cam.zoom_speed * elapsed_seconds);
		proj_changed = true;
	}
	if (glfwGetKey (window, GLFW_KEY_EQUAL)) {
		cam.Zoom(-cam.zoom_speed * elapsed_seconds);
		proj_changed = true;
	}
	// Update projection matrix
	if (proj_changed) 
	{
		GLuint projMat_loc = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(projMat_loc, 1, GL_FALSE, cam.ProjectionMatrix());
	}
}

BezierSurface* CreateBezierSurf()
{
	std::vector<Vector3> control1Points;
	control1Points.push_back( Vector3( 0.0f, 0.0f, 0.0f) );
	control1Points.push_back( Vector3( 3.0f, 0.0f, 1.0f) );
	control1Points.push_back( Vector3( 6.0f, 10.0f, 0.0f) );
	control1Points.push_back( Vector3( 10.0f, 0.0f, 5.0) );

	std::vector<Vector3> control2Points;
	control2Points.push_back( Vector3( 0.0f, 0.0f, 0.0f) );
	control2Points.push_back( Vector3( 1.0f, 3.0f, 10.0f) );
	control2Points.push_back( Vector3( 0.0f, 6.0f, 1.0f) );
	control2Points.push_back( Vector3( 0.0f, 10.0f, 0.0) );

	BezierSurface* temp = new BezierSurface(program, control1Points, control2Points);
	return temp;
}

void GenMoonDemo()
{
	NUM_BODIES = 3;
	Body* earth;
	Body* moon;
	Body* theSun = new Body(.5f, Vector3(0, 0, 0), Vector3(0,0,0), SUN, program);
	theSun->Init();

	bodies = std::vector<Body*>();
	bodies.push_back(theSun);

	earth = new Body(.05f, Vector3(0, 0, 0), Vector3(0,0,0), PLANET, program);
	float semiMajLMult = 1;
	float minRadius = 10;
	earth->SetOrbit(*theSun, 
		minRadius, 
		Vector3(((rand() % 200 + 1) - 100)*.001f, ((rand() % 200 + 1) - 100)*.001f, ((rand() % 200 + 1) - 100)*.001f), 
		0, 
		semiMajLMult);
	earth->Init();
	bodies.push_back(earth);

	moon = new Body(.01f, Vector3(0, 0, 0), Vector3(0,0,0), ASTEROID, program);
	semiMajLMult = 1;
	minRadius = earth->radius + moon->radius;
	moon->SetOrbit(*earth, 
		minRadius*2, 
		Vector3(((rand() % 200 + 1) - 100)*.001f, ((rand() % 200 + 1) - 100)*.001f, ((rand() % 200 + 1) - 100)*.001f), 
		0, 
		semiMajLMult);
	moon->Init();
	bodies.push_back(moon);
}

void GenTwoBody()
{
	NUM_BODIES = 2;

	bodies.clear();
	
	Body* x = new Body(1.f, Vector3(-5,0,0), Vector3(0, 0, 0), PLANET, program);
	x->Init();
	bodies.push_back(x);

	Body* y = new Body(1.f, Vector3(5.f,0.f,0.f), Vector3(0.f, 0.001f, -0.005), PLANET, program);
	y->Init();
	bodies.push_back(y);
}

void GenSystem()
{
	NUM_BODIES = 7;
	Body* p1;
	Body* theSun = new Body(3, Vector3(0, 0, 0), Vector3(0,0,0), SUN, program);
	theSun->Init();
	bodies = std::vector<Body*>();
	bodies.push_back(theSun);

	for(int i = 1; i < NUM_BODIES; i++)
	{
		float rankScalar = static_cast<float>(i*.1f); // further away a planet is, higher the value

		p1 = new Body((.2f) + ((rand() % 10 + 1) - (5-rankScalar*2.f))*.015f, Vector3(0, 0, 0), Vector3(0,0,0), PLANET, program);
		//p1 = new Body((.2f) + ((rand() % 10 + 1))*.01f, Vector3(0, 0, 0), Vector3(0,0,0), PLANET, program);
		bodies.push_back(p1);
		float semiMajLMult = (rand() % 4+1);
		if(semiMajLMult <= 4)
		{
			semiMajLMult = 1;
		}
		else
		{
			semiMajLMult -= 3; // from 0->1
			semiMajLMult *= 4; // from 0->4
			semiMajLMult += 1; // from 1->5
		}
		
		float minRadius;
		if (i > 1)
			minRadius = Vector3::dist(theSun->pos, bodies[i - 1]->pos) + bodies[i-1]->radius + p1->radius;
		else
			minRadius = theSun->radius + p1->radius;

		p1->SetOrbit(*theSun, 
			minRadius + (rand() % 10 + 1)*(rankScalar), 
			Vector3(((rand() % 200 + 1) - 100)*.001f, 1 + ((rand() % 200 + 1) - 100)*.001f, ((rand() % 200 + 1) - 100)*.001f), 
			0, 
			semiMajLMult);
		p1->Init();
	}
}

void CalcGravity()
{
	Vector3 totalG = Vector3(0, 0, 0);
	Body* curr;
	Body* currPuller;
	Vector3 toCurr(0,0,0);
	for (int j = 0; j < NUM_BODIES; j++) // Start at index = 1, because bodies[0] == theSun
	{
		curr = bodies[j];
		for (int i = 0; i < NUM_BODIES; i++) // Calc gravitational pull from all bodies, except self
		{
			//GRAVITY!!!
			currPuller = bodies[i];
			if (curr != currPuller && currPuller->type != ASTEROID) // no gravity between self and self
			{
				float distBetween = Vector3::dist(curr->pos, currPuller->pos);

				if (distBetween > .01)//this.radius + curr.radius) // else intersecting
				{
					float force = G*((curr->mass*currPuller->mass)/(distBetween*distBetween));
					float gravAccel = force/curr->mass; // I think... ?
					toCurr = currPuller->pos - curr->pos;
					toCurr = Vector3::normalize(toCurr);
					toCurr *= gravAccel;
					totalG += toCurr;
				}
			}
		}
		curr->accel = totalG;
		totalG = Vector3(0,0,0);
	}

	// Rigorous collision detection (sphere-sphere)
	for(int i = 0; i < NUM_BODIES; i++)
	{
		for(int j = i + 1; j < NUM_BODIES; j++)
		{
			PlanetCollRes(*bodies[i], *bodies[j]);
		}
	}
}

void AsteroidAttack()
{
	float numAsteroids = 100;
	int oldNumBodies = NUM_BODIES;
	NUM_BODIES += numAsteroids;
	Vector3 astPos;
	for (int i = 0; i < numAsteroids; i++)
	{
		astPos.x = rand()%100 - 50;
		astPos.y = rand()%100 - 50;
		astPos.z = rand()%100 - 50;
		astPos = Vector3::normalize(astPos);
		astPos *=  20;
		bodies.push_back(new Body((.05f) + ((rand() % 10 + 1))*.004f, astPos, Vector3(0,0,0), ASTEROID, program));
		(bodies[oldNumBodies + i])->Init();
	}
}

// Based off of sphere collision from - 
void PlanetCollRes(Body& a, Body& b)
{
	float dist = Vector3::dist(a.pos, b.pos);
	if (dist < a.radius + b.radius) //they are intersecting
	{
		float overlap = (a.radius + b.radius) - dist;
		Vector3 toB = b.pos - a.pos;

		// Fixes pos, so balls are just barely contacting
		Vector3 move = Vector3::normalize(toB);
		move *= overlap / 2.f;
		b.pos += move;
		move *= -1;
		a.pos += move;

		// First, find the normalized vector n from the center of 
		// circle1 to the center of circle2
		Vector3 n = b.pos - a.pos; //c1, c2
		n = Vector3::normalize(n);
		// Find the length of the component of each of the movement
		// vectors along n. 
		// a1 = v1 . n
		// a2 = v2 . n
		float a1 = Vector3::dot(b.vel,n);
		float a2 = Vector3::dot(a.vel,n);

		// Using the optimized version, 
		// optimizedP =  2(a1 - a2)
		//              -----------
		//                m1 + m2
		float optimizedP = (2.0 * (a1 - a2)) / (b.mass + a.mass);

		// Calculate v1', the new movement vector of circle1
		// v1' = v1 - optimizedP * m2 * n
		Vector3 bNew = b.vel - (n * (optimizedP * a.mass));
		b.vel = bNew;

		// Calculate v1', the new movement vector of circle1
		// v2' = v2 + optimizedP * m1 * n
		Vector3 aNew = a.vel + (n * (optimizedP * b.mass));
		a.vel = aNew;
	}
}