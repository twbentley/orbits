#include "Cube.h"
#include "Sphere.h"

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

// Forward initialization
void Initialize();
void CreateShape();
void Display();
void keyboard();//unsigned char key, int mouseX, int mouseY);
void TryCircle();
void ResolveCol(Shape& a, Shape&b);

int main(int argc, char **argv)
{
	// GLUT
	/*glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutCreateWindow( "A5 Polyhedrons (Bounding Buckets)" );*/

	// Set up glfw and display window
	if(glfwInit() == 0) return 1;
	window = glfwCreateWindow(512, 512, "orBITs", NULL, NULL);
	if(window == nullptr) return 1;
	glfwMakeContextCurrent(window);

	// Set up glew
	glewExperimental = GL_TRUE;
	glewInit();
	
	Initialize();

	//glutKeyboardFunc( keyboard );
	//glutDisplayFunc( Display );
	
	//glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	//glutMainLoop();

	// GLFW "game loop"
	while(!glfwWindowShouldClose(window))
	{
		Display();
		keyboard();
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

	_CrtDumpMemoryLeaks();
}

// Initialize world
void Initialize()
{
	// Set world size in 3 dimensions
	WORLD_SIZE = Vector3(100.f,100.0f,100.f);

	// Load shaders and use resulting shader program
	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	
	Cube* cube = new Cube( 0.15f, Vector3(0.f,0.f,0.f), Vector3(0.7f,0.7f,0.f));
	cube->Init(program);
	Sphere* sphere = new Sphere( 0.2f, Vector3(0.f,0.f,0.f), Vector3(0.f,0.f,0.f));
	sphere->Init(program);

	shapes = new Shape*[NUM_OBJECTS];
	shapes[0] = cube;
	shapes[1] = sphere;

	glEnable(GL_DEPTH_TEST);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);
	//glClearColor(0.1f, 0.1f, 0.1f, 0.01f);
	glClearColor(1.0,1.0,1.0,1.0);
}

// Display objects
void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	glDisableClientState(GL_VERTEX_ARRAY);
	//glutSwapBuffers();
	//glutPostRedisplay();
	glfwSwapBuffers(window);
}

// Handle keyboard input
void keyboard()//unsigned char key, int mouseX, int mouseY)
{
	if(glfwGetKey(window, 'Q')) // Quit Game
		glfwSetWindowShouldClose(window, true);
	if(glfwGetKey(window, 'Z'))
		Matrix4::UpdateRotationMatrix(shapes[0]->rotMatrix, 'z', 2.f);
	if(glfwGetKey(window, 'Y'))
		Matrix4::UpdateRotationMatrix(shapes[0]->rotMatrix, 'y', 2.f);
	if(glfwGetKey(window, 'X'))
		Matrix4::UpdateRotationMatrix(shapes[0]->rotMatrix, 'x', 2.f);
}