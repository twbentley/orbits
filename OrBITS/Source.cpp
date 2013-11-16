#include "Cube.h"
#include "Sphere.h"

// Define for leak detection
#define _CRTDBG_MAP_ALLOC

// Prevent console window from opening
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

// Shaper program
GLuint program;
// Shapes in world
Shape** shapes;
// Number of objects in world
int NUM_OBJECTS = 2;

// Forward initialization
void Initialize();
void CreateShape();
void Display();
void keyboard(unsigned char key, int mouseX, int mouseY);
void TryCircle();
void ResolveCol(Shape& a, Shape&b);

int main(int argc, char **argv)
{
	//srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//glEnable(GL_CULL_FACE);
	glutInitWindowSize(512, 512);
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutCreateWindow( "A5 Polyhedrons (Bounding Buckets)" );
	glewExperimental = GL_TRUE;
	glewInit();
	Initialize();

	glutKeyboardFunc( keyboard );
	glutDisplayFunc( Display );
	
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutMainLoop();

	// Delete objects;
	for(int i = 0; i < NUM_OBJECTS; i++)
	{
		delete shapes[i];
	}
	// Delete array
	delete [] shapes;

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
	glutSwapBuffers();
	glutPostRedisplay();
}

// Handle keyboard input
void keyboard(unsigned char key, int mouseX, int mouseY)
{
	switch( key )
	{
	case 033 : case 'q' : case 'Q':	// Quit game
		//exit( EXIT_SUCCESS );
		glutLeaveMainLoop();
		break;
	case 'z' : case 'Z':
		Matrix4::UpdateRotationMatrix(shapes[0]->rotMatrix, 'z', 2.f);
	case 'y' : case 'Y':
		Matrix4::UpdateRotationMatrix(shapes[0]->rotMatrix, 'y', 2.f);
	case 'x' : case 'X':
		Matrix4::UpdateRotationMatrix(shapes[0]->rotMatrix, 'x', 2.f);
	default:
		break;
	}
}