#ifndef MATRIX4_H
#define MATRIX4_H

#include "Vector4.h"
#include <string>
#include <sstream>
//#include <ostream>
//#include <istream>

// Define value of pi
#define PI 3.14159265358979323846264338327950288

class Matrix4
{
public:
	Vector4 matrix[4];

	// Constructors and Destructor
	Matrix4(void);
	Matrix4(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& d);
	Matrix4( GLfloat m00, GLfloat m10, GLfloat m20, GLfloat m30,
	  GLfloat m01, GLfloat m11, GLfloat m21, GLfloat m31,
	  GLfloat m02, GLfloat m12, GLfloat m22, GLfloat m32,
	  GLfloat m03, GLfloat m13, GLfloat m23, GLfloat m33 );
	Matrix4(const Matrix4& m);
	~Matrix4(void);

	// Copy assignment
	Matrix4& operator = (const Matrix4& other);

	// Indexers
	Vector4& operator [] ( int i );
	const Vector4& operator [] ( int i ) const;

	// Non-Modifying Arithmatic Operators
	Matrix4 operator + (const Matrix4& m) const;
	Matrix4 operator - (const Matrix4& m) const;
	Matrix4 operator * (const GLfloat s) const;
	Matrix4 operator * (const Matrix4& m) const;
	friend Matrix4 operator * (const GLfloat s, const Matrix4& m);
	Matrix4 operator / (const GLfloat s) const;

	// Modifying Arithmatic Operators
	Matrix4& operator += (const Matrix4& m);
	Matrix4& operator -= (const Matrix4& m);
	Matrix4& operator *= (const GLfloat s);
	Matrix4& operator *= (const Matrix4& m);
	Matrix4& operator /= (const GLfloat s);

	// Matrix/Array(row) Operator
	Vector4 operator * (const Vector4& v) const;

	// Insertion and Extraction Operators
	//friend ostream& operator << ( ostream& os, const Matrix4& m);
	//friend istream& operator >> (istream& is, Matrix4& m);

	// Conversion Operators
	operator const GLfloat* () const;
	operator GLfloat* ();

	// Non-Class Matrix4 Methods
	Matrix4 matrixCompMult(const Matrix4& A, const Matrix4& B);
	Matrix4 transpose();

	std::string ToString();

	static Matrix4 CreatePositionMatrix(float, float, float);
	static Matrix4 CreateProjectionMatrix(float, float, float, float, float, float);
	static void SetPositionMatrix(Matrix4&, float, float, float);
	static void UpdatePositionMatrix(Matrix4&, float, float, float);
	static void UpdateRotationMatrix(Matrix4&, char, float);
	static void Matrix4::UpdateScaleMatrix(Matrix4&, float, float, float);
	static void Matrix4::UpdateSkewMatrix(Matrix4& currMatrix, Vector4);
	float zAngle;
	Vector4 skewAngle;
};

#endif