#version 150 

in  vec4 vPosition;
in vec2 vertexUV;

out vec2 UV;

uniform mat4 vRotate;
uniform mat4 vTrans;
uniform mat4 projection;
uniform mat4 view;

void main() 
{
	//gl_Position = projection * view * vTrans * vRotate * vPosition;

	// Pull in transformations and rotations
	gl_Position = vTrans * vRotate * vPosition;

	// Pass UV values (currently issue where UVs are also bound to vertex data)
	UV = vertexUV;
} 