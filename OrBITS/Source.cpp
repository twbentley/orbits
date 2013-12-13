#include "Cube.h"
#include "Sphere.h"
#include "BezierSurface.h"
#include "Camera.h"
#include "Button.h"
#include <queue>

// Define for leak detection
#define _CRTDBG_MAP_ALLOC

// Prevent console window from opening
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

enum STATE { MENU, PLAY };

// GLFW window
GLFWwindow* window;
// Shader program
GLuint program;
// Shader program for buttons/menu
GLuint button_program;
// Shapes in world
Shape** shapes;
BezierSurface* bezier;
Button* button;
// Number of objects in world
int NUM_OBJECTS = 2;

Camera cam;
float prevSeconds;

GLuint prevMouseState;
GLuint currMouseState;
STATE gameState = MENU;

unsigned char * data;
GLuint textureID;
double cursorX;
double cursorY;

// Constants for window size
const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 512;

// Forward initialization
void Initialize();
void CreateShape();
BezierSurface* CreateBezierSurf(); // WOOOAH, TOTALLY AWESOME DUDE!
void Display();
void Input();
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

	if(gameState == MENU)
	{
		button->Update();
		button->Render();
	}
	else if(gameState == PLAY)
	{
		// Update and render all objects
		for (int i = 0; i <  NUM_OBJECTS; i++)
		{
			shapes[i]->Update();
			shapes[i]->Render();
		}

		bezier->Display();

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

// Handle keyboard and mouse input
void Input()
{
	prevMouseState = currMouseState;
	currMouseState = glfwGetMouseButton(window, 0);

	if(glfwGetKey(window, GLFW_KEY_ESCAPE)) // Quit Game
		glfwSetWindowShouldClose(window, true);

	if(gameState == PLAY)
		cameraInputCheck();
	else
	{
		if(prevMouseState == GLFW_PRESS && currMouseState == GLFW_RELEASE)// && (cursorX < button->vertices[1].x && cursorX > button->vertices[3].x) )
		{
			// Get cursor position (0,0 is upper left hand corner)
			glfwGetCursorPos(window, &cursorX, &cursorY);

			// Convert cursor position to openGL coordinates
			cursorX = (cursorX / SCREEN_WIDTH) * 2.0 - 1.0;
			cursorY = (cursorY / SCREEN_HEIGHT);
			cursorY = (cursorY * 2.0 - 1.0) * -1.0;

			std::cout << cursorX << " | " << cursorY << std::endl;

			// If button is clicked
			if( (cursorX < button->vertices[1].x && cursorX > button->vertices[3].x) && (cursorY < button->vertices[1].y && cursorY > button->vertices[3].x) )
			{
				//std::cout << "HELLOOO!!!!!" << std::endl;
				//button->Click();

				gameState = PLAY;

				//std::cout << button->vertices[1] << std::endl;
			}
		}
	}
}