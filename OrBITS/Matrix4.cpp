#include "Matrix4.h"

using namespace std;
#include <iostream>

// Default Constructor (Diagonal/Identity) Matrix
Matrix4::Matrix4(void)
{
	matrix[0][0] = 1.0f;
	matrix[1][1] = 1.0f;
	matrix[2][2] = 1.0f;
	matrix[3][3] = 1.0f;

	zAngle = 0.0f;
}

// Full Parameterized Constructor
Matrix4::Matrix4( GLfloat m00, GLfloat m01, GLfloat m02, GLfloat m03,
	  GLfloat m10, GLfloat m11, GLfloat m12, GLfloat m13,
	  GLfloat m20, GLfloat m21, GLfloat m22, GLfloat m23,
	  GLfloat m30, GLfloat m31, GLfloat m32, GLfloat m33 )
{
	matrix[0][0] = m00;
	matrix[0][1] = m01;
	matrix[0][2] = m02;
	matrix[0][3] = m03;
	matrix[1][0] = m10;
	matrix[1][1] = m11;
	matrix[1][2] = m12;
	matrix[1][3] = m13;
	matrix[2][0] = m20;
	matrix[2][1] = m21;
	matrix[2][2] = m22;
	matrix[2][3] = m23;
	matrix[3][0] = m30;
	matrix[3][1] = m31;
	matrix[3][2] = m32;
	matrix[3][3] = m33;

	zAngle = 0.0f;
}

// Parameterized Constructors for Vectors
Matrix4::Matrix4(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& d)
{
	matrix[0] = a;
	matrix[1] = b;
	matrix[2] = c;
	matrix[3] = d;

	zAngle = 0.0f;
}

// Copy Constructor
Matrix4::Matrix4(const Matrix4& m)
{
	matrix[0] = m.matrix[0];
	matrix[1] = m.matrix[1];
	matrix[2] = m.matrix[2];
	matrix[3] = m.matrix[3];

	zAngle = 0.0f;
}

// Destructor
Matrix4::~Matrix4(void) { }

// Copy assignment
Matrix4& Matrix4::operator = (const Matrix4& other)
{
	matrix[0] = other.matrix[0];
	matrix[1] = other.matrix[1];
	matrix[2] = other.matrix[2];
	matrix[3] = other.matrix[3];

	return *this;
}

// Indexer operators
Vector4& Matrix4::operator [] (int i)
{ 
	return matrix[i]; 
}
const Vector4& Matrix4::operator [] (int i) const
{ return matrix[i]; }

// Matrix-Matrix addition (non-modifying)
Matrix4 Matrix4::operator + (const Matrix4& m) const
{
	return Matrix4
	(
		matrix[0] + m.matrix[0],
		matrix[1] + m.matrix[1],
		matrix[2] + m.matrix[2],
		matrix[3] + m.matrix[3]
	);
}

// Matrix-Matrix subtraction (non-modifying)
Matrix4 Matrix4::operator - (const Matrix4& m) const
{
	return Matrix4
	(
		matrix[0] - m.matrix[0],
		matrix[1] - m.matrix[1],
		matrix[2] - m.matrix[2],
		matrix[3] - m.matrix[3]
	);
}

// Matrix-Scalar multiplication (non-modifying)
Matrix4 Matrix4::operator * (const GLfloat s) const
{
	return Matrix4
	(
		matrix[0][0] * s,
		matrix[1][0] * s,
		matrix[2][0] * s,
		matrix[3][0] * s,
		matrix[0][1] * s,
		matrix[1][1] * s,
		matrix[2][1] * s,
		matrix[3][1] * s,
		matrix[0][2] * s,
		matrix[1][2] * s,
		matrix[2][2] * s,
		matrix[3][2] * s,
		matrix[0][3] * s,
		matrix[1][3] * s,
		matrix[2][3] * s,
		matrix[3][3] * s
	);
}

// Matrix-Matrix multiplication (non-modifying)
Matrix4 Matrix4::operator * (const Matrix4& m) const
{
	Matrix4 a(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			for(int k = 0; k < 4; k++)
			{
				a[i][j] += matrix[i][k] * m.matrix[k][j];
				//a[i][j] += _m[i][k] * m[k][j];
			}
		}
	}
	
	//cout << a.ToString();
	return a;
}

// Matrix-Scalar division (non-modifying)
Matrix4 Matrix4::operator / (const GLfloat s) const
{
	// Don't forget about divide-by-zero

	GLfloat r = GLfloat(1.0) / s;
	return *this * r;
	//Why not: 'return *this / s;' ?
}

// Matrix-Matrix addition (modifying)
Matrix4& Matrix4::operator += (const Matrix4& m)
{
	matrix[0] += m.matrix[0];
	matrix[1] += m.matrix[1];
	matrix[2] += m.matrix[2];
	matrix[3] += m.matrix[3];

	return *this;
}

// Matrix-Matrix subtraction (modifying)
Matrix4& Matrix4::operator -= (const Matrix4& m)
{
	matrix[0] -= m.matrix[0];
	matrix[1] -= m.matrix[1];
	matrix[2] -= m.matrix[2];
	matrix[3] -= m.matrix[3];

	return *this;
}

// Matrix-Scalar multiplication (modifying)
Matrix4& Matrix4::operator *= (const GLfloat s)
{
	matrix[0] *= s;
	matrix[1] *= s;
	matrix[2] *= s;
	matrix[3] *= s;

	return *this;
}

// Matrix-Matrix multiplication (modifying)
Matrix4& Matrix4::operator *= (const Matrix4& m)
{
	// Initialize a temporary Matrix4 to be initialized with the new values
	Matrix4 a(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	// Perform matrix(4x4) multiplication and store in temp matrix
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			for(int k = 0; k < 4; k++)
			{
				a[i][j] += matrix[i][k] * m.matrix[k][j];
				//a[i][j] += _m[i][k] * m[k][j];
			}
		}
	}

	// Point this matrix at the new one (with new values)
	return *this = a;
}

// Matrix-Scalar division (modifying)
Matrix4& Matrix4::operator /= (const GLfloat s)
{
	GLfloat r = GLfloat(1.0) / s;
	return *this *= r;
}

// Matrix-Row multiplication (non-modifying)
Vector4 Matrix4::operator * (const Vector4& v) const
{
	return
	(
		Vector4
		(
			matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2] * v.z + matrix[0][3] * v.w,
			matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2] * v.z + matrix[1][3] * v.w,
			matrix[2][0] * v.x + matrix[2][1] * v.y + matrix[2][2] * v.z + matrix[2][3] * v.w,
			matrix[3][0] * v.x + matrix[3][1] * v.y + matrix[3][2] * v.z + matrix[3][3] * v.w
		)
	);
}

// What are these for?
Matrix4::operator const GLfloat* () const
{
	return static_cast<const GLfloat*>( &( matrix[0].x) );
}
Matrix4::operator GLfloat* () 
{
	return static_cast<GLfloat*>( &( matrix[0].x) );
}

// Not sure what this actually is
Matrix4 matrixCompMult(const Matrix4& A, const Matrix4& B)
{
	return Matrix4
	(
		A.matrix[0][0] * B.matrix[0][0], A.matrix[0][1] * B.matrix[0][1], A.matrix[0][2] * B.matrix[0][2], A.matrix[0][3] * B.matrix[0][3],
		A.matrix[1][0] * B.matrix[1][0], A.matrix[1][1] * B.matrix[1][1], A.matrix[1][2] * B.matrix[1][2], A.matrix[1][3] * B.matrix[1][3],
		A.matrix[2][0] * B.matrix[2][0], A.matrix[2][1] * B.matrix[2][1], A.matrix[2][2] * B.matrix[2][2], A.matrix[2][3] * B.matrix[2][3],
		A.matrix[3][0] * B.matrix[3][0], A.matrix[3][1] * B.matrix[3][1], A.matrix[3][2] * B.matrix[3][2], A.matrix[3][3] * B.matrix[3][3]
	);
}

// Transpose the given matrix
Matrix4 Matrix4::transpose()
{
	return Matrix4
	(
		matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
		matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
		matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
		matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]
	);
}

// Return a string version of the Matrix
string Matrix4::ToString()
{
	ostringstream stream;

	for(int i = 0; i < 4; i++)
	{
		stream << "| ";
		stream << matrix[i].x;
		stream << " ";
		stream << matrix[i].y;
		stream << " ";
		stream << matrix[i].z;
		stream << " ";
		stream << matrix[i].w;
		stream << " |";
		stream << endl;
	}

	return stream.str();
}

Matrix4 Matrix4::CreatePositionMatrix(float positionX, float positionY, float positionZ)
{
	return Matrix4(1.0f, 0.0f, 0.0f, positionX, 0.0f, 1.0f, 0.0f, positionY, 0.0f, 0.0f, 1.0f, positionZ, 0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4 Matrix4::CreateProjectionMatrix(float left,float right,float bottom,float top,float zNear,float zFar)
{
	float tx=-(right+left)/(right-left);
	float ty=-(top+bottom)/(top-bottom);
	float tz=-(zFar+zNear)/(zFar-zNear);

	return Matrix4(2.0f / right, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f / -bottom, 0.0f, 0.0f, 0.0f, 0.0f, -2.0f / (zFar - zNear), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

void Matrix4::SetPositionMatrix(Matrix4& currMatrix, float positionX, float positionY, float positionZ)
{
	currMatrix.matrix[0][3] = positionX;
	currMatrix.matrix[1][3] = positionY;
	currMatrix.matrix[2][3] = positionZ;
}

void Matrix4::UpdatePositionMatrix(Matrix4& currMatrix, float positionX, float positionY, float positionZ)
{
	currMatrix.matrix[0][3] += positionX;
	currMatrix.matrix[1][3] += positionY;
	currMatrix.matrix[2][3] += positionZ;
}

void Matrix4::UpdateRotationMatrix(Matrix4& currMatrix, char axisToRotate, float angleInDegrees)
{
	float angleInRadians = angleInDegrees * PI / 180;

	if(axisToRotate == 'z' || axisToRotate == 'Z')
	{
		currMatrix.zAngle += angleInDegrees;

		currMatrix *= Matrix4(	
								cos(angleInRadians), -sin(angleInRadians), 0.0f, 0.0f, 
								sin(angleInRadians), cos(angleInRadians), 0.0f, 0.0f, 
								0.0f, 0.0f, 1.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f
							 );
	}
	else if(axisToRotate == 'y' || axisToRotate == 'Y')
	{
		currMatrix.zAngle += angleInDegrees;

		currMatrix *= Matrix4(	
								cos(angleInRadians), 0.0f, sin(angleInRadians), 0.0f, 
								0.0f, 1.0f, 0.0f, 0.0f, 
								-sin(angleInRadians), 0.0f, cos(angleInRadians), 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f
							 );
	}
	else if(axisToRotate == 'x' || axisToRotate == 'X')
	{
		currMatrix.zAngle += angleInDegrees;

		currMatrix *= Matrix4(	
								1.0f, 0.0f, 0.0f, 0.0f, 
								0.0f, cos(angleInRadians), -sin(angleInRadians), 0.0f, 
								0.0f, sin(angleInRadians), cos(angleInRadians), 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f
							 );
	}
}

void Matrix4::UpdateScaleMatrix(Matrix4& currMatrix, float scaleX, float scaleY, float scaleZ)
{
	currMatrix.matrix[0][0] = scaleX;
	currMatrix.matrix[1][1] = scaleY;
	currMatrix.matrix[2][2] = scaleZ;
}

void Matrix4::UpdateSkewMatrix(Matrix4& currMatrix, Vector4 skew)
{
	currMatrix.skewAngle = skew;
	currMatrix.matrix[1][0] = tan(currMatrix.skewAngle.x);
	currMatrix.matrix[0][1] = tan(currMatrix.skewAngle.y);
}