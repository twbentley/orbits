// Based off of Angels vec4 functionality

#include "Vector4.h"

// Default Constructor
Vector4::Vector4()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

// Parameterized Constructor
Vector4::Vector4(GLfloat px, GLfloat py, GLfloat pz, GLfloat pw)
{
	x = px;
	y = py;
	z = pz;
	w = pw;
}

// Copy Constructor
Vector4::Vector4(const Vector4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

// Destructor
Vector4::~Vector4() { }

// Copy assignment
Vector4& Vector4::operator = (const Vector4& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;

	return *this;
}

GLfloat& Vector4::operator[](int i)
{
	return *(&x + i);
}
const GLfloat Vector4::operator[](int i) const
{
	return *(&x + i);
}

// Vector value negation operator (return Vector using new values)
Vector4 Vector4::operator - () const
{
	return Vector4( -x, -y, -z, -w);
}
// Vector-Vector addition operator (return Vector using new values)
Vector4 Vector4::operator + ( const Vector4& v ) const
{
	return Vector4( x + v.x, y + v.y, z + v.z, w + v.w );
}
// Vector-Vector subtraction operator (return Vector using new values)
Vector4 Vector4::operator - ( const Vector4& v ) const
{
	return Vector4( x - v.x, y - v.y, z - v.z, w - v.w );
}
// Vector-scalar multiplication operator (return Vector using new values)
Vector4 Vector4::operator * ( const GLfloat s ) const
{
	return Vector4(x * s, y * s, z * s, w * s);
}
// Vector-Vector multiplication operator (return Vector using new values)
Vector4 Vector4::operator * ( const Vector4& v ) const
{
	return Vector4(v.x * x, v.y * y, v.z * z, v.w + w);
}
// Vector-scalar division operator
Vector4 Vector4::operator / ( const GLfloat s ) const
{
	GLfloat temp = GLfloat(1.0) / s;
	return *this * temp;
}

// Vector-Vector addition applied to this object
Vector4& Vector4::operator += ( const Vector4& v )
{ 
	x += v.x;  y += v.y;  z += v.z;  w += v.w; return *this; 
}
// Vector-Vector subtraction applied to this object
Vector4& Vector4::operator -= ( const Vector4& v )
{ 
	x -= v.x; y -= v.y; z -= v.z; w -= v.z; return *this; 
}
// Vector-scalar multiplication applied to this object
Vector4& Vector4::operator *= ( const GLfloat s )
{ 
	x *= s; y *= s; z *= s; w *= s;  return *this; 
}
// Vector-Vector multiplication applied to this object
Vector4& Vector4::operator *= ( const Vector4& v )
{ 
	x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; 
}
// Vector-scalar division applied to this object
Vector4& Vector4::operator /= ( const GLfloat s )
{
	GLfloat r = GLfloat(1.0) / s;
	*this *= r;

	return *this;
}

// Vector-Vector dot product
GLfloat Vector4::dot(const Vector4& v) const
{
	return this->x * v.x + this->y * v.y + this->z * v.z + this->w * v.w;
}
GLfloat Vector4::dot() const
{
	return this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w;
}
// Vector length
GLfloat Vector4::length() const
{
	return std::sqrt(abs(dot()));
}
// Vector normalize
Vector4 Vector4::normalize( const Vector4& v ) const
{
	// Can't divide by 0
	return v / length();
}
// Vector-Vector crossproduct
Vector4 Vector4::cross(const Vector4& a, const Vector4& b ) const
{
    return Vector4
	(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x,
		0.0f
	);
}