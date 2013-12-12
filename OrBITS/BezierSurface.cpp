#include "BezierSurface.h"

//using namespace glm;
using namespace std;

#pragma region Static variables
const Vector4 BezierSurface::_controlPointsColor1 = Vector4( 0.0f, 0.0f, 1.0f, 1.0f );
const Vector4 BezierSurface::_controlLinesColor1  = Vector4( 1.0f, 0.3f, 1.0f, 1.0f );

const Vector4 BezierSurface::_controlPointsColor2 = Vector4( 0.0f, 1.0f, 0.0f, 1.0f );
const Vector4 BezierSurface::_controlLinesColor2  = Vector4( 0.4f, 1.0f, 1.0f, 1.0f );
#pragma endregion

#pragma region Constructor
BezierSurface::BezierSurface(GLuint myShaderProgram, std::vector<Vector3> points1, std::vector<Vector3> points2, unsigned __int8 numberOfTimeStamps, Vector4 bottomLeftColor, Vector4 bottomRightColor, Vector4 topLeftColor, Vector4 topRightColor )
	: _controlPoints1( points1 ), _controlPoints2( points2 ), _numberOfTimeStamps( numberOfTimeStamps ), _bottomLeftColor( bottomLeftColor ), _bottomRightColor( bottomRightColor ), _topLeftColor( topLeftColor ), _topRightColor( topRightColor )
{
	this->myShaderProgram = myShaderProgram;

	// Create a vertex array object
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

	Matrix4::UpdatePositionMatrix(transMatrix, 0, 0, 0);

	// Preallocate size of vectors
	_pointsOnTheSurface = vector<vector<Vector3>>( _numberOfTimeStamps + 1 );
	_vertices	= vector<Vector3>( _numberOfTimeStamps * _numberOfTimeStamps * 12 );	// 12 triangles made (6) each side
	_colors		= vector<Vector4>( _numberOfTimeStamps * _numberOfTimeStamps * 12 );	// 12 triangles made

#pragma region Fill Colors for Control Points and Lines
	int controlPointsSize1 = _controlPoints1.size();
	_colorControlPoints1 = vector<Vector4>( controlPointsSize1 );
	_colorControlLines1 = vector<Vector4>( controlPointsSize1 );


	int controlPointsSize2 = _controlPoints2.size();
	_colorControlPoints2 = vector<Vector4>( controlPointsSize2 );
	_colorControlLines2 = vector<Vector4>( controlPointsSize2 );

#pragma endregion

	fillColors();

#pragma region Setup Points of the Surface
	glGenBuffers( 1, &_curveVertexbuffer );
	glBindBuffer( GL_ARRAY_BUFFER, _curveVertexbuffer );
	glBufferData( GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vector3) + _colors.size() * sizeof(Vector4),
		NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(Vector3), &_vertices[0] );
	glBufferSubData( GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vector3), _colors.size() * sizeof(Vector4), &_colors[0] );
#pragma endregion

#pragma region Setup Control Points and Lines of  (1)
	glGenBuffers( 1, &_controlVertexbuffer1 );
	glBindBuffer( GL_ARRAY_BUFFER, _controlVertexbuffer1 );
	glBufferData( GL_ARRAY_BUFFER, _controlPoints1.size()*sizeof(Vector3) + (_colorControlPoints1.size() + _colorControlLines1.size()) * sizeof(Vector4),
		NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, _controlPoints1.size() * sizeof(Vector3), &_controlPoints1[0] );
	glBufferSubData( GL_ARRAY_BUFFER, _controlPoints1.size() * sizeof(Vector3), _colorControlPoints1.size() * sizeof(Vector4), &_colorControlPoints1[0] );
	glBufferSubData( GL_ARRAY_BUFFER, _controlPoints1.size() * sizeof(Vector3) + _colorControlPoints1.size() * sizeof(Vector4), _colorControlLines1.size() * sizeof(Vector4), &_colorControlLines1[0] );
#pragma endregion

#pragma region Setup Control Points and Lines of  (2)
	glGenBuffers( 1, &_controlVertexbuffer2 );
	glBindBuffer( GL_ARRAY_BUFFER, _controlVertexbuffer2 );
	glBufferData( GL_ARRAY_BUFFER, _controlPoints2.size()*sizeof(Vector3) + (_colorControlPoints2.size() + _colorControlLines2.size()) * sizeof(Vector4),
		NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, _controlPoints2.size() * sizeof(Vector3), &_controlPoints2[0] );
	glBufferSubData( GL_ARRAY_BUFFER, _controlPoints2.size() * sizeof(Vector3), _colorControlPoints2.size() * sizeof(Vector4), &_colorControlPoints2[0] );
	glBufferSubData( GL_ARRAY_BUFFER, _controlPoints2.size() * sizeof(Vector3) + _colorControlPoints2.size() * sizeof(Vector4), _colorControlLines2.size() * sizeof(Vector4), &_colorControlLines2[0] );
#pragma endregion

	createPoints();

	// set up vertex arrays
	GLuint loc = glGetAttribLocation( myShaderProgram, "vPosition" );
	glEnableVertexAttribArray( loc );
	glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	// Set value of rotation for this object
	GLuint vRotateLoc = glGetUniformLocation(myShaderProgram, "vRotate");
	glUniformMatrix4fv(vRotateLoc, 1, GL_TRUE, (GLfloat*)rotMatrix);

	// Set value of translation for this object
	GLuint vTransLoc = glGetUniformLocation(myShaderProgram, "vTrans");
	glUniformMatrix4fv(vTransLoc, 1, GL_TRUE, (GLfloat*)transMatrix);

	// Enable value of color for this object
	GLuint vfColorLoc = glGetAttribLocation( myShaderProgram, "vfColor" ); 
	glEnableVertexAttribArray( vfColorLoc );
	glVertexAttribPointer( vfColorLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(_vertices.size() * sizeof(Vector3)) );
	glUniform4f(vfColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
}
#pragma endregion

#pragma region Deconstructor
BezierSurface::~BezierSurface(void)
{
	glDeleteBuffers( 1, &_curveVertexbuffer );
	glDeleteBuffers( 1, &_controlVertexbuffer1 );
	glDeleteBuffers( 1, &_controlVertexbuffer2 );
}
#pragma endregion

// Create points on the Curve
#pragma region Create Points
void BezierSurface::createPoints()
{
	unsigned __int8 CONTROL_POINT1_COUNT = _controlPoints1.size();
	unsigned __int8 CONTROL_POINT2_COUNT = _controlPoints2.size();
	unsigned __int8 N1 = CONTROL_POINT1_COUNT - 1;
	unsigned __int8 N2 = CONTROL_POINT2_COUNT - 1;

	unsigned __int8 i,j, ii, jj;			

	// Time Step through the Curve
	const float timeStepIncrement = 1.0f / _numberOfTimeStamps;
	float currentTimeStep1 = 0;	
	float currentTimeStep2 = 0;	

	// Factorials 
	float n1Factorial = factorial(N1);
	float n2Factorial = factorial(N2);

	Vector3 tempVector;
	Vector3 control1CalcedVector;
	std::vector<Vector3> tempVectorList( _numberOfTimeStamps + 1 );

	// Fill in the columns of the surface with each row
	for( i = 0; i < _numberOfTimeStamps + 1; ++i )
	{
		tempVector				= Vector3( 0.0f, 0.0f, 0.0f);
		control1CalcedVector	= Vector3( 0.0f, 0.0f, 0.0f);


		// Calculate Control1 Points ( Horizontal row of the Bezier Surface )
		for( j = 0; j < CONTROL_POINT1_COUNT; ++j )
		{					// n! / !i * (n - i)!
			control1CalcedVector += _controlPoints1[j] * ( ( n1Factorial / (factorial( j ) * factorial( N1 - j ) ) ) * 
				static_cast<float>( pow( ( 1 - currentTimeStep1 ),  N1 - j ) * pow( currentTimeStep1, j ) ) );
		}			

		// Influence the Curve caluclated above with the other curve
		for(ii = 0; ii < _numberOfTimeStamps + 1; ++ii )
		{
			tempVector = control1CalcedVector;
			for(jj = 0; jj < CONTROL_POINT2_COUNT; ++jj )
			{					// n! / !i * (n - i)!
				tempVector += _controlPoints2[jj] * ( ( n2Factorial / ( factorial( jj ) * factorial( N2 - jj ) ) ) * 
					static_cast<float>( pow( ( 1 - currentTimeStep2),  N2 - jj ) * pow( currentTimeStep2, jj ) ) );
			}

			// Fill in the row of the surface
			tempVectorList[ii] = tempVector;
			currentTimeStep2 += timeStepIncrement;
		}

		// Fill in a column with the rows of the surface and loop again
		_pointsOnTheSurface[i] = tempVectorList;
		currentTimeStep2 = 0;
		currentTimeStep1 += timeStepIncrement;
	}

	fillBuffer();
}
#pragma endregion

#pragma region Fill Buffer
void BezierSurface::fillBuffer()
{
	int index = 0;

	Vector4 horizontal;

	// Create Triangles based on the Calculated Surfaces
	for(unsigned __int8 i = 0; i < _numberOfTimeStamps; ++i )
	{
		for(unsigned __int8 j = 0; j < _numberOfTimeStamps; ++j )
		{
			
			_vertices[index] = _pointsOnTheSurface[i][j];			++index;
			_vertices[index] = _pointsOnTheSurface[i][j + 1];		++index;
			_vertices[index] = _pointsOnTheSurface[i + 1][j + 1];	++index;
			
			_vertices[index] = _pointsOnTheSurface[i + 1][j + 1];	++index;
			_vertices[index] = _pointsOnTheSurface[i + 1][j];		++index;
			_vertices[index] = _pointsOnTheSurface[i][j];			++index;
								
			
			// Render other side flipped
			_vertices[index] = _pointsOnTheSurface[i + 1][j + 1]	;	++index;
			_vertices[index] = _pointsOnTheSurface[i][j + 1]		;	++index;
			_vertices[index] = _pointsOnTheSurface[i][j]			;	++index;

			_vertices[index] = _pointsOnTheSurface[i][j]			 ;	++index;
			_vertices[index] = _pointsOnTheSurface[i + 1][j]		 ;	++index;
			_vertices[index] = _pointsOnTheSurface[i + 1][j + 1]	 ;	++index;
			
		}
	}

	// Update for when we 'Move'(recalculate) the vertices
	glBindBuffer( GL_ARRAY_BUFFER, _curveVertexbuffer );
	glBufferSubData( GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(Vector3), &_vertices[0] );
	
	glBindBuffer( GL_ARRAY_BUFFER, _controlVertexbuffer1 );
	glBufferSubData( GL_ARRAY_BUFFER, 0, _controlPoints1.size() * sizeof(Vector3), &_controlPoints1[0] );

	glBindBuffer( GL_ARRAY_BUFFER, _controlVertexbuffer2 );
	glBufferSubData( GL_ARRAY_BUFFER, 0, _controlPoints2.size() * sizeof(Vector3), &_controlPoints2[0] );
}
#pragma endregion

#pragma region Fill Colors
void BezierSurface::fillColors()
{

	int index = 0;

	// Calulate Current Color of Triangle Vertice based on a 'texture' formed by the corner colors
	for(unsigned __int8 i = 0; i < _numberOfTimeStamps; ++i )
	{
		for(unsigned __int8 j = 0; j < _numberOfTimeStamps; ++j )
		{
			Vector4 ij		= getColor( i, j,			_numberOfTimeStamps, _numberOfTimeStamps );
			Vector4 ij_1		= getColor( i, j + 1,		_numberOfTimeStamps, _numberOfTimeStamps );
			Vector4 i_1j_1	= getColor( i + 1, j + 1,	_numberOfTimeStamps, _numberOfTimeStamps );
			Vector4 i_1j		= getColor( i + 1, j,		_numberOfTimeStamps, _numberOfTimeStamps );

			
			_colors[index] = ij;		++index;
			_colors[index] = ij_1;		++index;
			_colors[index] = i_1j_1;	++index;

			_colors[index] = i_1j_1;	++index;
			_colors[index] = i_1j;		++index;
			_colors[index] = ij;		++index;
			
			
			// Render other side flipped
			_colors[index] = ij;		++index;
			_colors[index] = ij_1;		++index;
			_colors[index] = i_1j_1;	++index;

			_colors[index] = i_1j_1;	++index;
			_colors[index] = i_1j;		++index;
			_colors[index] = ij;		++index;
			
		}
	}

	// Color Control Points and Lines
	int controlPointSize1 = _controlPoints1.size();
	for( int i = 0; i < controlPointSize1; ++i )
	{
		_colorControlPoints1[i] = _controlPointsColor1;
		_colorControlLines1[i]  = _controlLinesColor1;
	}

	int controlPointSize2 = _controlPoints2.size();
	for( int i = 0; i < controlPointSize2; ++i )
	{
		_colorControlPoints2[i] = _controlPointsColor2;
		_colorControlLines2[i]  = _controlLinesColor2;
	}
}
#pragma endregion

// Get color based on the corner colors
#pragma region Get Color
Vector4 BezierSurface::getColor( int widthIndex, int heightIndex, int widthSize, int heightSize )
{
	float widthScale  = widthIndex / static_cast<float>( widthSize );
	float heightScale = heightIndex / static_cast<float>( heightSize );

	Vector4 colorTotal1 = _bottomLeftColor * ( 1 - widthScale ) + _bottomRightColor * ( widthScale );
	Vector4 colorTotal2 = _topLeftColor * ( 1 - widthScale ) + _topRightColor * ( widthScale );
	
	Vector4 colorFinal = colorTotal1 * ( 1 - heightScale ) + colorTotal2 * ( heightScale );
	colorFinal.w = 1.0f;	// make sure its visible
	return colorFinal;

}
#pragma endregion

// Display the Curve
#pragma region Display
void BezierSurface::Display() 
{
	#pragma region Render Bezier Surface
	glUseProgram(myShaderProgram);
	glBindBuffer( GL_ARRAY_BUFFER, _curveVertexbuffer );
	glBindVertexArray(vao);

	//Matrix4::UpdateRotationMatrix(rotMatrix, 'z', 1.0f);

	GLuint loc = glGetAttribLocation(myShaderProgram, "vPosition");
	glEnableVertexAttribArray( loc );
	glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0 ,BUFFER_OFFSET(0) );

	GLuint loc2 = glGetAttribLocation(myShaderProgram, "vfColor");
	glEnableVertexAttribArray( loc2 );
	glVertexAttribPointer( loc2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( _vertices.size() * sizeof(Vector3)) );

	// Set value of rotation for this object
	GLuint vRotateLoc = glGetUniformLocation(myShaderProgram, "vRotate");
	glUniformMatrix4fv(vRotateLoc, 1, GL_TRUE, (GLfloat*)rotMatrix);

	// Set value of translation for this object
	GLuint vTransLoc = glGetUniformLocation(myShaderProgram, "vTrans");
	glUniformMatrix4fv(vTransLoc, 1, GL_TRUE, (GLfloat*)transMatrix);

	glDrawArrays( GL_TRIANGLES, 0, _vertices.size() );

#pragma endregion
	#pragma region WE DONT HAVE A DEBUG MODE --- IRRELEVANT FOR NOW
	/*if( debugVisible )
	{
	#pragma region Render Control  Lines (1)
	glBindBuffer( GL_ARRAY_BUFFER, _controlVertexbuffer1 );

	// set up vertex arrays
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0) );

	glEnableVertexAttribArray( vColor );

	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET( ( _controlPoints1.size() + _colorControlPoints1.size() ) * sizeof(glm::vec4)) );

	glUniformMatrix4fv( model, 1, GL_TRUE, &_model[0][0] );

	glDrawArrays( GL_LINE_STRIP, 0, _controlPoints1.size() );

	glDisableVertexAttribArray( vPosition );
	glDisableVertexAttribArray( vColor );
#pragma endregion

	#pragma region Render Control Points (1)
	glBindBuffer( GL_ARRAY_BUFFER, _controlVertexbuffer1 );

	// set up vertex arrays
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0) );

	glEnableVertexAttribArray( vColor );

	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET( _controlPoints1.size() * sizeof(glm::vec4)) );

	glUniformMatrix4fv( model, 1, GL_TRUE, &_model[0][0] );

	glDrawArrays( GL_POINTS, 0, _controlPoints1.size() );

	glDisableVertexAttribArray( vPosition );
	glDisableVertexAttribArray( vColor );
#pragma endregion

	#pragma region Render Control  Lines (2)
	glBindBuffer( GL_ARRAY_BUFFER, _controlVertexbuffer2 );

	// set up vertex arrays
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0) );

	glEnableVertexAttribArray( vColor );

	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET( ( _controlPoints2.size() + _colorControlPoints2.size() ) * sizeof(glm::vec4)) );

	glUniformMatrix4fv( model, 1, GL_TRUE, &_model[0][0] );

	glDrawArrays( GL_LINE_STRIP, 0, _controlPoints2.size() );

	glDisableVertexAttribArray( vPosition );
	glDisableVertexAttribArray( vColor );
#pragma endregion

	#pragma region Render Control Points (2)
	glBindBuffer( GL_ARRAY_BUFFER, _controlVertexbuffer2 );

	// set up vertex arrays
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0) );

	glEnableVertexAttribArray( vColor );

	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET( _controlPoints2.size() * sizeof(glm::vec4)) );

	glUniformMatrix4fv( model, 1, GL_TRUE, &_model[0][0] );

	glDrawArrays( GL_POINTS, 0, _controlPoints2.size() );

	glDisableVertexAttribArray( vPosition );
	glDisableVertexAttribArray( vColor );
#pragma endregion
	}*/
	#pragma endregion
}
#pragma endregion

// Factorial function
int BezierSurface::factorial(const int num)
{
	int total = 1;
	for (int i = 1; i < num; i++)
	{
		total *= i;
	}
	return total;
}

