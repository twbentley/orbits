#include "Sphere.h"

Sphere::Sphere(void) : Shape() { }

// Parameterized constructor
Sphere::Sphere(GLfloat width, Vector3 vel, Vector3 pos) : Shape(width, vel, pos)
{
	nRings = 13;
	nSegments = 13;
	radius = width;// / 2;

	//NUM_POINTS = (nRings + 1) * (nSegments + 1);
	NUM_POINTS = (nRings * nSegments) * 2 * 3;
	NUM_VERTS = nRings * nSegments;
}

Sphere::~Sphere(void) { }

// Initialize sphere
void Sphere::Init(GLuint program)
{
	vertices = new Vector3[NUM_VERTS];
	points = new Vector3[NUM_POINTS];
	base_colors = new Vector4[NUM_VERTS];
	colors = new Vector4[NUM_POINTS];

	float modifier = 0.0f;
	for(int i = 0; i < NUM_POINTS; i++)
	{
		if(i % (NUM_POINTS / nRings) == 0)
			modifier = static_cast<float>(i) / static_cast<float>(NUM_POINTS);
		colors[i] = Vector4(1.0f * modifier, 1.0f * modifier, 1.0f * modifier, 1.0f); // red
	}

	GenerateVertices();
	GenerateTriangles();
	SetPoints();

	InitOpenGL(program);
}

// Update specific for a cube
void Sphere::Update()
{
	// Rotation, Forces and other physics updates
	//Matrix4::UpdateRotationMatrix(rotMatrix, 'z', 1.f);
	//Matrix4::UpdateRotationMatrix(rotMatrix, 'y', 1.5f);
	//Matrix4::UpdateRotationMatrix(rotMatrix, 'x', -0.25f);
	//Matrix4::SetPositionMatrix(transMatrix, 0.01f, 0.f, 0.01f);

	// Default update
	Shape::Update();

	// Update rotation and translation for cube
	GLuint vRotateLoc = glGetUniformLocation(myShaderProgram, "vRotate");
	glUniformMatrix4fv(vRotateLoc, 1, GL_FALSE, (GLfloat*)rotMatrix);
	GLuint vTransLoc = glGetUniformLocation(myShaderProgram, "vTrans");
	glUniformMatrix4fv(vTransLoc, 1, GL_FALSE, (GLfloat*)transMatrix);
}

void Sphere::GenerateVertices()
{
	// from an ogre 3d example http://www.ogre3d.org/tikiwiki/ManualSphereMeshes&structure=Cookbook

	Vector3* verts = new Vector3[(nRings + 1) * (nSegments + 1)];
	GLuint* indices = new GLuint[6 * nRings * (nSegments+1)];
	GLfloat deltaRingAngle = (GLfloat)(PI/nRings);
	GLfloat deltaSegAngle = (GLfloat)(2 * PI / nSegments);
	GLushort verticeIndex = 0;
	Vector3* tv = verts;
	GLuint* ti = indices;
	
	// Generate the group of rings for the sphere
	for (GLuint ring = 0; ring <= nRings; ring++)
	{
		GLfloat r0 = radius * sinf(ring * deltaRingAngle);
		GLfloat y0 = radius * cosf(ring * deltaRingAngle);

		rings.push_back(Ring());

		// Generate the group of segments for the current ring
		for (GLuint seg = 0; seg <= nSegments; seg++)
		{
			GLfloat x0 = r0 * sinf(seg * deltaSegAngle);
			GLfloat z0 = r0 * cosf(seg * deltaSegAngle);
			// add one vertex to the strip that makes up the sphere
			Vector3 pos(x0, y0, z0);			
			int index = (ring * (nRings+1)) + seg;
			verts[index][0] = pos[0];
			verts[index][1] = pos[1];
			verts[index][2] = pos[2];
			rings[ring].points.push_back(verts[index]);

			/*verts[index].position = pos;
			verts[index].normal = pos.Normalized();
			verts[index].color = Vector4::One;
			verts[index].tex = Vector3((GLfloat)seg/(GLfloat)nSegments, (GLfloat)ring/(GLfloat)nRings, 0.0f);*/
			if (ring != nRings)
			{
				// each vertex (except the last) has six indices pointing to it
				*ti++ = verticeIndex + nSegments + 1;
				*ti++ = verticeIndex;
				*ti++ = verticeIndex + nSegments;
				*ti++ = verticeIndex + nSegments + 1;
				*ti++ = verticeIndex + 1;
				*ti++ = verticeIndex;
				verticeIndex++;
			}
		}
	}

	//points = verts;

	//for(int i = 0; i < rings.size(); i++)
	//{
	//	//std::cout << (points[i])[0] << " " << (points[i])[1] << " " << (points[i])[2] << std::endl;
	//	for(int j = 0; j < rings[i].points.size(); j++)
	//	{
	//		std::cout << "Ring|Segment: " << i << " | " << j << " | " << rings[i].points[j].x << " | " << rings[i].points[j].y << " | " << rings[i].points[j].z << std::endl;
	//	}
	//}
}

void Sphere::GenerateTriangles()
{
	for(int currRing = 0; currRing < nRings; currRing++)
	{
		for(int currPt = 0; currPt < nSegments; currPt++)
		{
			if(currPt + 1 < nSegments)
			{
				Triangle t1  = { rings[currRing].points[currPt] , rings[currRing+1].points[currPt], rings[currRing].points[currPt+1] };
				Triangle t2 = { rings[currRing].points[currPt+1], rings[currRing+1].points[currPt], rings[currRing+1].points[currPt+1] };
				triangles.push_back(t1);
				triangles.push_back(t2);
			}
			else
			{
				Triangle t1 = { rings[currRing].points[currPt],   rings[currRing+1].points[currPt], rings[currRing].points[0] };
				Triangle t2 = { rings[currRing].points[0], rings[currRing+1].points[currPt], rings[currRing+1].points[0] };
				triangles.push_back(t1);
				triangles.push_back(t2);
			}
		}
	}
}

void Sphere::SetPoints()
{
	for(int i = 0; i < NUM_POINTS; i = i + 3)
	{
		points[i] = triangles[i/3].point1;
		points[i+1] = triangles[i/3].point2;
		points[i+2] = triangles[i/3].point3;
	}

	//for(int j = 0; j < NUM_POINTS; j++)
	//{
	//	std::cout << points[j].x << " | " <<  points[j].y << " | " <<  points[j].x << std::endl; 
	//}

}

/*
*	Tesselation Blog entry: http://prideout.net/blog/?p=48
*	//Compute top cap 
		// a 'tent-like' cap from ring 1's y value with triangles to ring 2's points
		~
	// If odd numRings i = 1, if even i = 0;
	for(int currRing = 0||1; currRing < numRings - 1||numRings - 2; currRing++)
	{
		for(int currPt = 0; currPt < numSegments; currPt++)
		{
			if(currPt + 1 < numSegments)
			{
				sphereTriangles.push(new Triangle(rings[currRing].points[currPt],   rings[currRing+1].points[currPt], rings[currRing].points[currPt+1] ));
				sphereTriangles.push(new Triangle(rings[currRing].points[currPt+1], rings[currRing+1].points[currPt], rings[currRing+1].points[currPt+1] ));
			}
			else
			{
				sphereTriangles.push(new Triangle(rings[currRing].points[currPt],   rings[currRing+1].points[currPt], rings[currRing].points[0]  );
				sphereTriangles.push(new Triangle(rings[currRing].points[0], rings[currRing+1].points[currPt], rings[currRing+1].points[0]);
			}
		}
	}

	//Compute bot cap
		// a 'tent-like' cap from last ring's y value with triangles to second-to-last ring's points
*
*/