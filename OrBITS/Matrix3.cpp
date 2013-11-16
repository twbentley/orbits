#include "Matrix3.h"

using namespace std;

// Default Constructor (Diagonal/Identity) Matrix
Matrix3::Matrix3(void)
{
	matrix[0][0] = 1.0f;
	matrix[1][1] = 1.0f;
	matrix[2][2] = 1.0f;
}

// Full Parameterized Constructor
Matrix3::Matrix3(GLfloat m00, GLfloat m01, GLfloat m02,
	  GLfloat m10, GLfloat m11, GLfloat m12,
	  GLfloat m20, GLfloat m21, GLfloat m22)
{
	matrix[0][0] = m00;
	matrix[1][0] = m10;
	matrix[2][0] = m20;
	matrix[0][1] = m01;
	matrix[1][1] = m11;
	matrix[2][1] = m21;
	matrix[0][2] = m02;
	matrix[1][2] = m12;
	matrix[2][2] = m22;
}

// Parameterized Constructor for Vectors
Matrix3::Matrix3(const Vector3& a, const Vector3& b, const Vector3& c)
{
	matrix[0] = a;
	matrix[1] = b;
	matrix[2] = c;
}

// Copy Constructor
Matrix3::Matrix3(const Matrix3& m)
{
	matrix[0] = m.matrix[0];
	matrix[1] = m.matrix[1];
	matrix[2] = m.matrix[2];
}

// Destructor
Matrix3::~Matrix3(void)
{
}

// Indexer operators
Vector3& Matrix3::operator [] (int i)
{ return matrix[i]; }
const Vector3& Matrix3::operator [] (int i) const
{ return matrix[i]; }

// Matrix-Matrix addition (non-modifying)
Matrix3 Matrix3::operator + (const Matrix3& m) const
{
	return Matrix3
	(
		matrix[0] + m.matrix[0],
		matrix[1] + m.matrix[1],
		matrix[2] + m.matrix[2]
	);
}

// Matrix-Matrix subtraction (non-modifying)
Matrix3 Matrix3::operator - (const Matrix3& m) const
{
	return Matrix3
	(
		matrix[0] - m.matrix[0],
		matrix[1] - m.matrix[1],
		matrix[2] - m.matrix[2]
	);
}

// Matrix-Scalar multiplication (non-modifying)
Matrix3 Matrix3::operator * (const GLfloat s) const
{
	return Matrix3
	(
		matrix[0] * s,
		matrix[1] * s,
		matrix[2] * s
	);
}

// Matrix-Matrix multiplication (non-modifying)
Matrix3 Matrix3::operator * (const Matrix3& m) const
{
	Matrix3 a(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			for(int k = 0; k < 3; k++)
			{
				a[i][j] += matrix[i][k] * m.matrix[k][j];
				//a[i][j] += _m[i][k] * m[k][j];
			}
		}
	}

	return a;
}

// Matrix-Scalar division (non-modifying)
Matrix3 Matrix3::operator / (const GLfloat s) const
{
	// Don't forget about divide-by-zero

	GLfloat r = GLfloat(1.0) / s;
	return *this * r;
	//Why not: 'return *this / s;' ?
}

// Matrix-Matrix addition (modifying)
Matrix3& Matrix3::operator += (const Matrix3& m)
{
	matrix[0] += m.matrix[0];
	matrix[1] += m.matrix[1];
	matrix[2] += m.matrix[2];

	return *this;
}

// Matrix-Matrix subtraction (modifying)
Matrix3& Matrix3::operator -= (const Matrix3& m)
{
	matrix[0] -= m.matrix[0];
	matrix[1] -= m.matrix[1];
	matrix[2] -= m.matrix[2];

	return *this;
}

// Matrix-Scalar multiplication (modifying)
Matrix3& Matrix3::operator *= (const GLfloat s)
{
	matrix[0] *= s;
	matrix[1] *= s;
	matrix[2] *= s;

	return *this;
}

// Matrix-Matrix multiplication (modifying)
Matrix3& Matrix3::operator *= (const Matrix3& m)
{
	Matrix3 a(0, 0, 0, 0, 0, 0, 0, 0, 0);

	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			for(int k = 0; k < 3; k++)
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
Matrix3& Matrix3::operator /= (const GLfloat s)
{
	GLfloat r = GLfloat(1.0) / s;
	return *this *= r;
}

// Matrix-Row multiplication (non-modifying)
Vector3 Matrix3::operator * (const Vector3& v) const
{
	return
	(
		Vector3
		(
			matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2] * v.z,
			matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2] * v.z,
			matrix[2][0] * v.x + matrix[2][1] * v.y + matrix[2][2] * v.z
		)
	);
}

// What are these for?
Matrix3::operator const GLfloat* () const
{
	return static_cast<const GLfloat*>( &matrix[0].x );
}
Matrix3::operator GLfloat* () 
{
	return static_cast<GLfloat*>( &matrix[0].x );
}

// Not sure what this actually is
Matrix3 matrixCompMult(const Matrix3& A, const Matrix3& B)
{
	return Matrix3
	(
		A.matrix[0][0] * B.matrix[0][0],
		A.matrix[0][1] * B.matrix[0][1],
		A.matrix[0][2] * B.matrix[0][2],
		A.matrix[1][0] * B.matrix[1][0],
		A.matrix[1][1] * B.matrix[1][1],
		A.matrix[1][2] * B.matrix[1][2],
		A.matrix[2][0] * B.matrix[2][0],
		A.matrix[2][1] * B.matrix[2][1],
		A.matrix[2][2] * B.matrix[2][2]
	);
}

// Transpose the given matrix (non-modifying)
Matrix3 Matrix3::transpose()
{
	return Matrix3
	(
		matrix[0][0], matrix[1][0], matrix[2][0],
		matrix[0][1], matrix[1][1], matrix[2][1],
		matrix[0][2], matrix[1][2], matrix[2][2] 
	);
}

// Return a string version of the Matrix
string Matrix3::ToString()
{
	ostringstream stream;

	for(int i = 0; i < 3; i++)
	{
		stream << "| ";
		stream << matrix[i].x;
		stream << " ";
		stream << matrix[i].y;
		stream << " ";
		stream << matrix[i].z;
		stream << " |";
		stream << endl;
	}

	return stream.str();
}