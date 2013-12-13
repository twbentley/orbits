// Based off of Angels vec3 functionality

#include "Vector3.h"

// Default Constructor
Vector3::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

// Parameterized Constructor
Vector3::Vector3(GLfloat px, GLfloat py, GLfloat pz)
{
	x = px;
	y = py;
	z = pz;
}

// Copy Constructor
Vector3::Vector3(const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

// Destructor
Vector3::~Vector3() { }

// Copy assignment
Vector3& Vector3::operator=(const Vector3& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

GLfloat& Vector3::operator[](int i)
{
	return *(&x + i);
}
const GLfloat Vector3::operator[](int i) const
{
	return *(&x + i);
}

// Vector value negation operator (return Vector using new values)
Vector3 Vector3::operator - () const
{
	return Vector3( -x, -y, -z);
}
// Vector-Vector addition operator (return Vector using new values)
Vector3 Vector3::operator + ( const Vector3& v ) const
{
	return Vector3( x + v.x, y + v.y, z + v.z);
}
// Vector-Vector subtraction operator (return Vector using new values)
Vector3 Vector3::operator - ( const Vector3& v ) const
{
	return Vector3( x - v.x, y - v.y, z - v.z );
}
// Vector-scalar multiplication operator (return Vector using new values)
Vector3 Vector3::operator * ( const GLfloat s ) const
{
	return Vector3(s * x, s * y, s * z);
}
// Vector-Vector multiplication operator (return Vector using new values)
Vector3 Vector3::operator * ( const Vector3& v ) const	
{
	return Vector3(v.x * x, v.y * y, v.z * z);
}
// Vector-scalar division operator
Vector3 Vector3::operator / ( const GLfloat s ) const
{
	GLfloat temp = GLfloat(1.0) / s;
	return *this * temp;
}

// Vector-Vector addition applied to this object
Vector3& Vector3::operator += ( const Vector3& v )
{ 
	x += v.x;  y += v.y;  z += v.z;  return *this; 
}
// Vector-Vector subtraction applied to this object
Vector3& Vector3::operator -= ( const Vector3& v )
{ 
	x -= v.x;  y -= v.y;  z -= v.z;  return *this; 
}
// Vector-scalar multiplication applied to this object
Vector3& Vector3::operator *= ( const GLfloat s )
{ 
	x *= s;  y *= s;  z *= s;  return *this; 
}
// Vector-Vector multiplication applied to this object
Vector3& Vector3::operator *= ( const Vector3& v )
{ 
	x *= v.x;  y *= v.y;  z *= v.z;  return *this; 
}
// Vector-scalar division applied to this object
Vector3& Vector3::operator /= ( const GLfloat s )
{
	GLfloat r = GLfloat(1.0) / s;
	*this *= r;

	return *this;
}

// ToString output
void Vector3::ToString(std::ostream& o) const
{
	o << "Vector3 - X: " << x << " | Y: " << y << " | Z: " << z;
}

// Vector-Vector dot product
GLfloat Vector3::dot( const Vector3& u, const Vector3& v )
{
	return u.x*v.x + u.y*v.y + u.z*v.z;
}
// Vector length
GLfloat Vector3::length( const Vector3& v )
{
	return std::sqrt(dot(v,v));
}
// Vector normalize
Vector3 Vector3::normalize( const Vector3& v )
{
	return v / length(v);
}
// Vector-Vector crossproduct
Vector3 Vector3::cross(const Vector3& a, const Vector3& b )
{
    return Vector3
	(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}