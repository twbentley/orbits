#ifndef MATRIX3_H
#define MATRIX3_H

#include "Vector3.h"
#include <string>
#include <sstream>
//#include <ostream>
//#include <istream>

class Matrix3
{
public:
	Vector3 matrix[3];

	// Constructors and Destructor
	Matrix3(void);
	Matrix3::Matrix3(const Vector3& a, const Vector3& b, const Vector3& c);
	Matrix3(GLfloat m00, GLfloat m01, GLfloat m02,
	  GLfloat m10, GLfloat m11, GLfloat m12,
	  GLfloat m20, GLfloat m21, GLfloat m22);
	Matrix3(const Matrix3& m);
	~Matrix3(void);

	// Indexers
	Vector3& operator [] ( int i );
	const Vector3& operator [] ( int i ) const;

	// Non-Modifying Arithmatic Operators
	Matrix3 operator + (const Matrix3& m) const;
	Matrix3 operator - (const Matrix3& m) const;
	Matrix3 operator * (const GLfloat s) const;
	Matrix3 operator * (const Matrix3& m) const;
	friend Matrix3 operator * (const GLfloat s, const Matrix3& m);
	Matrix3 operator / (const GLfloat s) const;

	// Modifying Arithmatic Operators
	Matrix3& operator += (const Matrix3& m);
	Matrix3& operator -= (const Matrix3& m);
	Matrix3& operator *= (const GLfloat s);
	Matrix3& operator *= (const Matrix3& m);
	Matrix3& operator /= (const GLfloat s);

	// Matrix/Array(row) Operator
	Vector3 operator * (const Vector3& v) const;

	// Insertion and Extraction Operators
	//friend ostream& operator << ( ostream& os, const Matrix3& m);
	//friend istream& operator >> (istream& is, Matrix3& m);

	// Conversion Operators
	operator const GLfloat* () const;
	operator GLfloat* ();

	// Non-Class Matrix3 Methods
	Matrix3 matrixCompMult(const Matrix3& A, const Matrix3& B);
	Matrix3 transpose();

	std::string ToString();
};

#endif