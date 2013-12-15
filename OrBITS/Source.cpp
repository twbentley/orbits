#include "Cube.h"
#include "Sphere.h"
#include "BezierSurface.h"
#include "Camera.h"
#include "Button.h"
#include <random>

#include "Globals.h"
#include "Body.h"

// Define for leak detection
#define _CRTDBG_MAP_ALLOC

// Prevent console window from opening
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

// GLFW window
GLFWwindow* window;
// Shader program
GLuint program;
GLuint button_program;
// Shapes in world
Shape** shapes;

// Celestial Bodies
Body** bodies;
int NUM_BODIES = 10;

BezierSurface* bezier;
Button* button;
// Number of objects in world
int NUM_OBJECTS = 2;

Camera cam;
float prevSeconds;

GLuint frameBuffer;
unsigned char * data;
GLuint textureID;


// Forward initialization
void Initialize();
void CreateShape();
BezierSurface* CreateBezierSurf(); // WOOOAH, TOTALLY AWESOME DUDE!
void GenSystem();	// Inits solar system
void GenMoonDemo();	// Inits Sun, planet, moon
void CalcGravity(); // Updates gravitation on planets
void Display();
void Keyboard();
void TryCircle();
void ResolveCol(Shape& a, Shape&b);
GLuint loadBMP_custom(const char* imagePath);

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
		Keyboard();
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

void GenMoonDemo()
{
	NUM_BODIES = 2;
	Body* earth;
	Body* moon;
	Body* theSun = new Body(.5f, Vector3(0, 0, 0), Vector3(0,0,0), SUN, program);
	theSun->Init();
	bodies = new Body*[NUM_BODIES + 1]; // plus 1 for sun!
	bodies[0] = theSun;

	earth = new Body(.05f, Vector3(0, 0, 0), Vector3(0,0,0), PLANET, program);
	float semiMajLMult = 1;
	float minRadius = 10;
	earth->SetOrbit(*theSun, 
				minRadius, 
				Vector3(((rand() % 200 + 1) - 100)*.001f, ((rand() % 200 + 1) - 100)*.001f, ((rand() % 200 + 1) - 100)*.001f), 
				0, 
				semiMajLMult);
	earth->Init();
	bodies[1] = earth;

	moon = new Body(.01f, Vector3(0, 0, 0), Vector3(0,0,0), ASTEROID, program);
	semiMajLMult = 1;
	minRadius = earth->radius + moon->radius;
	moon->SetOrbit(*earth, 
					minRadius*2, 
					Vector3(((rand() % 200 + 1) - 100)*.001f, ((rand() % 200 + 1) - 100)*.001f, ((rand() % 200 + 1) - 100)*.001f), 
					0, 
					semiMajLMult);
	moon->Init();
	bodies[2] = moon;
}

void GenSystem()
{
	Body* p1;
	Body* theSun = new Body(3, Vector3(0, 0, 0), Vector3(0,0,0), SUN, program);
	theSun->Init();
	bodies = new Body*[NUM_BODIES + 1]; // plus 1 for sun!
	bodies[0] = theSun;

	for(int i = 1; i < NUM_BODIES + 1; i++)
	{
		float rankScalar = static_cast<float>(i*.1f); // further away a planet is, higher the value

		p1 = new Body((.5f) + ((rand() % 10 + 1) - (5-rankScalar*2.f))*.05f, Vector3(0, 0, 0), Vector3(0,0,0), ASTEROID, program);
		bodies[i] = p1;
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

		// the commented line should be correct, the bottom is easier to debug though, so we'll start with that
		//p1->SetOrbit(*theSun, 700 + (rand() % 800+1 - 400), Vector3(static_cast<float>((rand() % 200 - 100)/100.f),3 + static_cast<float>((rand() % 200 - 100)/100.f),static_cast<float>((rand() % 200 - 100)/100.f)), 0, semiMajLMult);
		float minRadius;
		if (i > 1)
		{
			minRadius = Vector3::dist(theSun->pos, bodies[i - 1]->pos) + bodies[i-1]->radius + p1->radius;
		}
		else
		{
			minRadius = theSun->radius + p1->radius;
		}
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
	for (int j = 1; j < NUM_BODIES + 1; j++) // Start at index = 1, because bodies[1] == theSun
	{
		curr = bodies[j];
		for (int i = 0; i < NUM_BODIES + 1; i++) // Calc gravitational pull from all bodies, except self
		{
			//GRAVITY!!!
			currPuller = bodies[i];
			if (curr != currPuller) // no gravity between self and self
			{
				float distBetween = Vector3::dist(curr->pos, currPuller->pos);

				if (distBetween > .01)//this.radius + curr.radius) // else intersecting
				{
					float force = G*((curr->mass*currPuller->mass)/(distBetween*distBetween));
					float gravAccel = force/curr->mass; // I think... ?
					Vector3 toCurr = currPuller->pos - curr->pos;
					toCurr = Vector3::normalize(toCurr);
					toCurr *= gravAccel;
					totalG += toCurr;
				}
			}
		}
		curr->accel = totalG;
		totalG = Vector3(0,0,0);
	}
}

// Initialize world
void Initialize()
{
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
	// Initialize the vertex position attribute from the vertex shader
    /*GLuint button_loc = glGetAttribLocation( button_program, "vPosition" );
    glEnableVertexAttribArray( button_loc );
    glVertexAttribPointer( button_loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );*/

	button = new Button(0.5f, 0.0f);
	button->Init(button_program);
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
	Sphere* sphere = new Sphere( 5.0f, Vector3(0.0f, 0.f, 0.f), Vector3(0.f, 0.f, 0.f));
	sphere->Init(program);

	bezier = CreateBezierSurf();

	shapes = new Shape*[NUM_OBJECTS];
	shapes[0] = cube;
	shapes[1] = sphere;

	//GenSystem();
	GenMoonDemo();

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

BezierSurface* CreateBezierSurf()
{
	std::vector<Vector3> control1Points;
	control1Points.push_back( Vector3( 0.0f, 0.0f, 0.0f) );
	control1Points.push_back( Vector3( 3.0f, 0.0f, 1.0f) );
	control1Points.push_back( Vector3( 6.0f, 1.0f, 0.0f) );
	control1Points.push_back( Vector3( 10.0f, 0.0f, 0.0) );

	std::vector<Vector3> control2Points;
	control2Points.push_back( Vector3( 0.0f, 0.0f, 0.0f) );
	control2Points.push_back( Vector3( 1.0f, 3.0f, 0.0f) );
	control2Points.push_back( Vector3( 0.0f, 6.0f, 1.0f) );
	control2Points.push_back( Vector3( 0.0f, 10.0f, 0.0) );

	BezierSurface* temp = new BezierSurface(program, control1Points, control2Points);
	return temp;
}

// Display objects
void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update and render all objects
	for (int i = 0; i <  NUM_OBJECTS; i++)
	{
		//shapes[i]->Update();
		//shapes[i]->Render();
	}

	//button->Update();
	//button->Render();
	//bezier->Display();

	CalcGravity();
	for (int i = 0; i < NUM_BODIES + 1; i++)
	{
		bodies[i]->Update();
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

	glfwSwapBuffers(window);
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

// Handle keyboard input
void Keyboard()//unsigned char key, int mouseX, int mouseY)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE)) // Quit Game
		glfwSetWindowShouldClose(window, true);

	cameraInputCheck();
}

GLuint loadBMP_custom(const char* imagePath)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	//unsigned char * data;

	// Open the file
	FILE * file;
	errno_t err;
	if( (err  = fopen_s( &file, imagePath, "rb" )) !=0 ){}

	if (file == NULL)                              
	{printf("Image could not be opened\n"); return 0;}
 
	if ( fread(header, 1, 54, file)!=54 ) // If not 54 bytes read : problem
	{
		printf("Not a correct BMP file\n");
		return false;
	}

	if ( header[0]!='B' || header[1]!='M' )
	{
		printf("Not a correct BMP file\n");
		return 0;
	}

	// Read ints from the byte array
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];
 
	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);
 
	//Everything is in memory now, the file can be closed
	fclose(file);
}
