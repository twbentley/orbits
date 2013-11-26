#include "Cube.h"
#include "Sphere.h"
#include "Camera.h"

// Define for leak detection
#define _CRTDBG_MAP_ALLOC

// Prevent console window from opening
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

// GLFW window
GLFWwindow* window;
// Shader program
GLuint program;
// Shapes in world
Shape** shapes;
// Number of objects in world
int NUM_OBJECTS = 2;

GLuint vao;
GLuint buffer;
Camera cam;
double prevSeconds;

// Constants for window size
const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 512;

// Forward initialization
void Initialize();
void CreateShape();
void Display();
void Keyboard();//unsigned char key, int mouseX, int mouseY);
void TryCircle();
void ResolveCol(Shape& a, Shape&b);

// TODO: Sync up all matrices and their new constructors

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

// Initialize world
void Initialize()
{
	// Set world size in 3 dimensions
	WORLD_SIZE = Vector3(100.f,100.0f,100.f);

	// Set up depth in OpenGL
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Vector3 points[4];
	points[0] = Vector3(-0.5f, -0.5f,  0.5f);
	points[1] = Vector3( 0.5f, -0.5f,  0.5f);
	points[2] = Vector3( 0.5f, -0.5f, -0.5f);
	points[3] = Vector3(-0.5f, -0.5f, -0.5f);

	// Create a vertex array object
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, 18 * sizeof(float), points, GL_STATIC_DRAW );

	// Set the initial camera position
	//cam.Reset();
	cam.Translate(0.0f, 0.0f, 2.0f);
	cam.SetViewportSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	// Get time
	prevSeconds = glfwGetTime();

	// Load shaders and use resulting shader program
	program = InitShader("vshader.glsl", "fshader.glsl");

	// Set up view and project matrices in shader
	GLuint viewMat_loc = glGetUniformLocation(program, "view");
	glUseProgram(program);
	glUniformMatrix4fv(viewMat_loc, 1, GL_FALSE, cam.ViewMatrix());
	GLuint projMat_loc = glGetUniformLocation(program, "projection");
	glUseProgram(program);
	glUniformMatrix4fv(program, 1, GL_FALSE, cam.ProjectionMatrix());
	
	Cube* cube = new Cube( 0.15f, Vector3(0.f,0.f,0.f), Vector3(0.7f,0.7f,0.f));
	cube->Init(program);
	Sphere* sphere = new Sphere( 0.2f, Vector3(0.f,0.f,0.f), Vector3(0.f,0.f,0.f));
	sphere->Init(program);

	shapes = new Shape*[NUM_OBJECTS];
	shapes[0] = cube;
	shapes[1] = sphere;

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

// Display objects
void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBindVertexArray(vao);
	glDrawArrays(GL_QUADS, 0, 4);

	// Update and render all objects
	for (int i = 0; i <  NUM_OBJECTS; i++)
	{
		shapes[i]->Update();
		shapes[i]->Render();
	}

	//// Resolve conflicts between all objects
	//for(int i = 0; i < NUM_OBJECTS; i++)
	//{
	//	for(int j = i + 1; j < NUM_OBJECTS; j++)
	//	{
	//		ResolveCol(*shapes[i], *shapes[j]);
	//	}
	//}

	//glDisableClientState(GL_VERTEX_ARRAY);
	glfwSwapBuffers(window);
}

void cameraInputCheck() {
	float cam_speed = 5.0f;	// Camera speed in units per second
	float cam_rotation_speed = 20.0f;	// Camera rotation speed in degrees per second

	double currentSeconds = glfwGetTime();
	double elapsed_seconds = currentSeconds - prevSeconds;
	prevSeconds = currentSeconds;

	Vector3 right(1.0f, 0.0f, 0.0f);
	Vector3 up(0.0f, 1.0f, 0.0f);
	Vector3 forward(0.0f, 0.0f, 1.0f);

	// control keys
	bool cam_moved = false;
	if (glfwGetKey (window, 'A')) {
		cam.Translate(cam.Right(), -cam_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, 'D')) {
		cam.Translate(cam.Right(), cam_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_PAGE_UP)) {
		cam.Translate(cam.Up(), cam_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_PAGE_DOWN)) {
		cam.Translate(cam.Up(), -cam_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, 'W')) {
		cam.Translate(cam.Forward(), -cam_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, 'S')) {
		cam.Translate(cam.Forward(), cam_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_UP)) {
		cam.Pitch(-cam_rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_DOWN)) {
		cam.Pitch(cam_rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_LEFT)) {
		cam.Yaw(-cam_rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, GLFW_KEY_RIGHT)) {
		cam.Yaw(cam_rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, 'Z')) {
		cam.Roll(cam_rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, 'X')) {
		cam.Roll(-cam_rotation_speed * elapsed_seconds);
		cam_moved = true;
	}
	if (glfwGetKey (window, ' ')) {
		cam.Reset();
		cam.Translate(0.0f, 0.0f, 2.0f);
		cam_moved = true;
	}
	// update view matrix
	if (cam_moved) {
		GLuint viewMat_loc = glGetUniformLocation(program, "view");
		glUniformMatrix4fv(viewMat_loc, 1, GL_FALSE, cam.ViewMatrix());
	}

	float cam_zoom_speed = 0.5f;
	bool proj_changed = false;
	if (glfwGetKey (window, GLFW_KEY_MINUS)) {
		cam.Zoom(cam_zoom_speed * elapsed_seconds);
		proj_changed = true;
	}
	if (glfwGetKey (window, GLFW_KEY_EQUAL)) {
		cam.Zoom(-cam_zoom_speed * elapsed_seconds);
		proj_changed = true;
	}
	// Update projection matrix
	if (proj_changed) {
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