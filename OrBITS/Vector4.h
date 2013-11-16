#ifndef VECTOR_4_H
#define VECTOR_4_H

#include <GL/glew.h>
//#include <GL/glfw.h>
#include <GL/glfw3.h>
#include <istream>

class Vector4
{
public:
	// Fields
	GLfloat x;
    GLfloat y;
	GLfloat z;
	GLfloat w;

	// Constructors
	Vector4();
    Vector4(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    Vector4(const Vector4& v);

	// Destructor
	~Vector4(void);

	// Copy assignment
	Vector4& operator = (const Vector4& other);

	// Indexers
	GLfloat& operator[](int i);
    const GLfloat operator[](int i) const;
	
	// Non-Modifying Arithmatic Operator Overloads
	Vector4 operator - () const;
    Vector4 operator + ( const Vector4& v ) const;
    Vector4 operator - ( const Vector4& v ) const;
    Vector4 operator * ( const GLfloat s ) const;
    Vector4 operator * ( const Vector4& v ) const;
	Vector4 operator / ( const GLfloat s ) const;

	// Modifying Arithmatic Operator Overloads
	Vector4& operator += ( const Vector4& v );
    Vector4& operator -= ( const Vector4& v );
    Vector4& operator *= ( const GLfloat s );
    Vector4& operator *= ( const Vector4& v );
    Vector4& operator /= ( const GLfloat s );

	// Non-class Vector4 Methods
	GLfloat dot(const Vector4&) const;
	GLfloat dot() const;
	GLfloat length() const;
	Vector4 normalize( const Vector4& v ) const;
	Vector4 cross(const Vector4& a, const Vector4& b) const;
};

#endif