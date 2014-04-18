// Need to include this before anything else
#include <vld.h>

#include "Cube.h"
#include "Sphere.h"
#include "BezierSurface.h"
#include "Camera.h"
#include "Button.h"
#include "Globals.h"
#include "Body.h"
#include "Container.h"
#include "Octree.h"

#include <vector>
#include <random>
#include <time.h>

// Define for leak detection
//#define _CRTDBG_MAP_ALLOC

// Prevent console window from opening
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

enum STATE { MENU, INSTRUCTIONS, PLAY, PAUSE, CREDITS };
enum GEN_STATE { SYSTEM, TWOBODY, MOON };

// GLFW window
GLFWwindow* window;
// Shader program
GLuint program;
// Shader program for buttons/menu
GLuint button_program;
// Celestial Bodies
std::vector<Body*> bodies;
int NUM_BODIES;

BezierSurface* bezier;

Octree* octree;
float timeUntilUpdate = 0;
int octreeCounter = 0;

Button* startButton;
Button* pauseImage;
Button* resetButton;
Button* asteroidButton;
Button* quitButton;
Button* instructions;
Button* credits;

Camera cam;
float prevSeconds;

GLuint prevMouseState;
GLuint currMouseState;
GLuint prevMouse2State;
GLuint currMouse2State;
GLuint prevPauseState;
GLuint currPauseState;
STATE gameState = MENU;
GEN_STATE systemState;


// Mouse cursor
double cursorX;
double cursorY;

// Octree
Container* container;
Vector3 qmax;
Vector3 qmin;

// Forward initialization
void Initialize();
void CreateShape();
BezierSurface* CreateBezierSurf();
void Display();
void Input();
void cameraInputCheck();
void TryCircle();

void ResolveCol(Shape& a, Shape&b);
void GenSystem();       // Inits solar system
void GenMoonDemo();     // Inits Sun, planet, moon
void GenTwoBody();		// Inits two planets - collision test
void DemoGenSwitch();	// Wrapper for switching which System Generation function to call
void CalcGravity();		// Updates gravitation on planets
void AsteroidAttack();	// Spawn number of asteroids surrounding Sun of set radius
void PlanetCollRes(Body& a, Body& b);

void SetupOctree();
void InitializeOctree();
void TestOctree();

void Cleanup();

int main()//int argc, char **argv)
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
	//SetupOctree();

	// GLFW "game loop"
	while(!glfwWindowShouldClose(window))
	{
		Input();
		Display();
		glfwPollEvents();
	}

	Cleanup();

	// Close and clean up glfw window
	glfwDestroyWindow(window);
	glfwTerminate();

	// Get memory leaks
	//_CrtDumpMemoryLeaks();
}

// Initialize world
void Initialize()
{
	srand(time(NULL));

	// Set up depth in OpenGL
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Set the initial camera position
	cam.SetInitialPosition(0.0f, 0.0f, 20.0f);
	cam.SetViewportSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	// Get current time
	prevSeconds = glfwGetTime();

	#pragma region Buttons 

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

	quitButton = new Button(0.5f, 0.0f, "button_quit");
	Matrix4::SetPositionMatrix(quitButton->transMatrix, -0.5f, -1.285f, 0.0f);
	quitButton->Init(button_program);

	instructions = new Button(0.5f * 2, 0.0f, "instructions");
	Matrix4::SetPositionMatrix(instructions->transMatrix, 0.5f - instructions->width/2, 0.5f - instructions->width/2, 0.0f);
	instructions->Init(button_program);

	credits = new Button(0.5f * 2, 0.0f, "credits");
	Matrix4::SetPositionMatrix(credits->transMatrix, 0.5f - credits->width/2, 0.5f - credits->width/2, 0.0f);
	credits->Init(button_program);

	#pragma endregion

	octree = new Octree(Vector3(-100.0f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f), 1);

	// Load shaders and use resulting shader program
	program = InitShader("vshader.glsl", "fshader.glsl");

	// Set up view and project matrices in shader
	GLuint viewMat_loc = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(viewMat_loc, 1, GL_FALSE, cam.ViewMatrix());
	GLuint projMat_loc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projMat_loc, 1, GL_FALSE, cam.ProjectionMatrix());

	bezier = CreateBezierSurf();

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	// Enable transparency
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set up background color
	//glClearColor(0.1f, 0.1f, 0.1f, 0.01f);
	glClearColor(1.0,1.0,1.0,1.0);
}

// Display objects and update them
void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(gameState == MENU)
	{
		startButton->Update();
		startButton->Render();

		bezier->Update();
		bezier->Display();
	}
	else if(gameState == INSTRUCTIONS)
	{
		instructions->Update();
		instructions->Render();
	}
	else if(gameState == PAUSE)
	{
		pauseImage->Update();
		pauseImage->Render();

		resetButton->Update();
		resetButton->Render();

		quitButton->Update();
		quitButton->Render();
	}
	else if(gameState == PLAY)
	{
		CalcGravity();

		asteroidButton->Update();
		asteroidButton->Render();
	}
	else if(gameState == CREDITS)
	{
		credits->Update();
		credits->Render();
	}

	if(gameState == PLAY || gameState == PAUSE)
	{
        for (int i = 0; i < NUM_BODIES; i++)
        {
			if(gameState == PLAY)
			{
				//octree->performUpdate(bodies, octree);
                bodies[i]->Update();
			}

            bodies[i]->Render();
        }
	}

	glfwSwapBuffers(window);
}

// Handle keyboard and mouse input
void Input()
{
	prevMouseState = currMouseState;
	currMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	prevMouse2State = currMouse2State;
	currMouse2State = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
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

	if(gameState == INSTRUCTIONS && (prevMouse2State == GLFW_PRESS && currMouse2State == GLFW_RELEASE) )
	{
		gameState = PLAY;
		systemState = SYSTEM;
		DemoGenSwitch();
	}

	if(prevMouseState == GLFW_PRESS && currMouseState == GLFW_RELEASE)
	{
		// Get cursor position (0,0 is upper left hand corner)
		glfwGetCursorPos(window, &cursorX, &cursorY);

		// Convert cursor position to openGL coordinates
		cursorX = (cursorX / SCREEN_WIDTH) * 2.0 - 1.0;
		cursorY = (cursorY / SCREEN_HEIGHT);
		cursorY = (cursorY * 2.0 - 1.0) * -1.0;
		
		// Start the game
		if(gameState == MENU && (cursorX < startButton->vertices[1].x && cursorX > startButton->vertices[3].x) && (cursorY < startButton->vertices[1].y && cursorY > startButton->vertices[3].x) )
		{
			gameState = INSTRUCTIONS;
		}
		else if(gameState == INSTRUCTIONS)
		{
			gameState = PLAY;

			// Change the assignment of systemState to change the gravity demo initialized
				//systemState = TWOBODY;
				//systemState = MOON;
				systemState = SYSTEM;

			DemoGenSwitch();
		}
		// Reset the camera and the world
		else if(gameState == PAUSE && (cursorX < resetButton->vertices[1].x + resetButton->transMatrix[0][3] && cursorX > resetButton->vertices[3].x + resetButton->transMatrix[0][3])
			&& (cursorY < resetButton->vertices[1].y + resetButton->transMatrix[1][3] && cursorY > resetButton->vertices[3].y + + resetButton->transMatrix[1][3]) )
		{
			cam.Reset();
			cam.position = Vector3(0,0,0);
			Cleanup();
			Initialize();
			DemoGenSwitch();
		}
		// Launch asteroids
		else if( gameState == PLAY && (cursorX < asteroidButton->vertices[1].x + asteroidButton->transMatrix[0][3] && cursorX > asteroidButton->vertices[3].x + asteroidButton->transMatrix[0][3])
			&& (cursorY < asteroidButton->vertices[1].y + asteroidButton->transMatrix[1][3] && cursorY > asteroidButton->vertices[3].y + + asteroidButton->transMatrix[1][3]) )
		{
			AsteroidAttack();
		}
		// Quit the game
		else if( gameState == PAUSE && (cursorX < quitButton->vertices[1].x + quitButton->transMatrix[0][3] && cursorX > quitButton->vertices[3].x + quitButton->transMatrix[0][3])
			&& (cursorY < quitButton->vertices[1].y + quitButton->transMatrix[1][3] && cursorY > quitButton->vertices[3].y + + quitButton->transMatrix[1][3]) )
		{
			gameState = CREDITS;
		}
		// Exit the program
		else if( gameState == CREDITS)
		{
			glfwSetWindowShouldClose(window, true);
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

// No leaks
void GenTwoBody()
{
	NUM_BODIES = 2;

	bodies.clear();
	
	Body* x = new Body(1.0f, Vector3(-5.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), PLANET, program);
	x->Init();
	bodies.push_back(x);

	Body* y = new Body(1.0f, Vector3(5.0f, 0.0f, 0.0f), Vector3(0.f, 0.001f, -0.005f), PLANET, program);
	y->Init();
	bodies.push_back(y);
}

// No leaks
void GenSystem()
{
	NUM_BODIES = 8;
	Body* p1;
	Body* theSun = new Body(3, Vector3(0, 0, 0), Vector3(0,0,0), SUN, program);
	theSun->Init();
	bodies.push_back(theSun);

	for(int i = 1; i < NUM_BODIES; i++)
	{
		float rankScalar = static_cast<float>(i * 0.1f); // further away a planet is, higher the value

		p1 = new Body((.2f) + ((rand() % 10 + 1) - (5-rankScalar*2.f))*.015f, Vector3(0, 0, 0), Vector3(0,0,0), PLANET, program);
		//p1 = new Body((.2f) + ((rand() % 10 + 1))*.01f, Vector3(0, 0, 0), Vector3(0,0,0), PLANET, program);
		bodies.push_back(p1);
		int semiMajLMult = (rand() % 4 + 1);
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
			Vector3(((rand() % 200 + 1) - 100)*.001f, .9f + ((rand() % 200 + 1) - 100)*.001f, ((rand() % 200 + 1) - 100)*.001f), 
			0, 
			semiMajLMult);
		p1->Init();

		// Clean up
		p1 = nullptr;
	}
	// Clean up
	theSun = nullptr;
}

void DemoGenSwitch()
{
	if(systemState == TWOBODY)
		GenTwoBody();
	else if(systemState == SYSTEM)
		GenSystem();
	else if (systemState == MOON)
		GenMoonDemo();
}

// No memory leaks
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

	// TODO: Call octree update!
	//if(octreeCounter >= 5)
	//{
		octree->advance(bodies, octree, 25 / 1000.0, timeUntilUpdate);
	//	octreeCounter = 0;
	//}
	//octreeCounter++;

	//// Rigorous collision detection (sphere-sphere)
	//for(int i = 0; i < NUM_BODIES; i++)
	//{
	//	for(int j = i + 1; j < NUM_BODIES; j++)
	//	{
	//		PlanetCollRes(*bodies[i], *bodies[j]);
	//	}
	//}
}

void AsteroidAttack()
{
	int numAsteroids = 50;
	int oldNumBodies = NUM_BODIES;
	NUM_BODIES += numAsteroids;
	Vector3 astPos;
	for (int i = 0; i < numAsteroids; i++)
	{
		astPos.x = static_cast<float>(rand() % 100 - 50);
		astPos.y = static_cast<float>(rand() % 100 - 50);
		astPos.z = static_cast<float>(rand() % 100 - 50);
		astPos = Vector3::normalize(astPos);
		astPos *=  20.0f;
		bodies.push_back(new Body((.05f) + ((rand() % 10 + 1))*.004f, astPos, Vector3(0,0,0), ASTEROID, program));
		(bodies[oldNumBodies + i])->Init();
	}
}

void Cleanup()
{
	// Deallocate buttons
	delete asteroidButton;
	delete startButton;
	delete resetButton;
	delete pauseImage;
	delete quitButton;
	delete instructions;
	delete credits;

	// Deallocate bezier
	delete bezier;

	// Deallocate octree
	delete octree;

	// Deallocate all bodies
	for(int i = 0; i < bodies.size(); i++)
		delete bodies[i];
	bodies.clear();
}


// Deprecated
void SetupOctree()
{
	InitializeOctree();
	TestOctree();

	delete container;
}
// Deprecated
void InitializeOctree()
{
	// Create a new Octree centered at the origin
	// with physical dimension 2x2x2
	//container = new Container(Vector3(0,0,0), Vector3(500,500,500), );

	//// Create a bunch of random points
	//const int nPoints = 1 * 1000 * 1000;
	//for(int i=0; i<nPoints; ++i) {
	//	points.push_back(randVec3());
	//}
	//printf("Created %ld points\n", points.size()); fflush(stdout);

	// Insert the points into the octree
	//octreePoints = new OctreePoint[nPoints];
	for(int i = 0; i < NUM_BODIES; i++)
	{
		//octreePoints[i].setPosition(points[i]);
		container->Insert(bodies[i]);
	}
	//printf("Inserted points to octree\n"); fflush(stdout);

	// Create a very small query box. The smaller this box is
	// the less work the octree will need to do. This may seem
	// like it is exagerating the benefits, but often, we only
	// need to know very nearby objects.
	qmin = Vector3(-50,-50,-50);
	qmax = Vector3(50,50,50);

	// Remember: In the case where the query is relatively close
	// to the size of the whole octree space, the octree will
	// actually be a good bit slower than brute forcing every point!
}
// Deprecated
void TestOctree()
{
	std::vector<Body*> results;
	container->getPointsInsideBox(qmin, qmax, results);

	//double T = stopwatch() - start;
	//printf("testOctree found %ld points in %.5f sec.\n", results.size(), T);
	std::cout << "testOctree found " << results.size() << " points" << std::endl;
}