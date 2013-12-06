#version 150 

in vec2 vertexUV;
out vec2 UV;

in  vec4 vPosition;
in vec4 vfColor;

out vec4 fragmentColor;

uniform mat4 vRotate;
uniform mat4 vTrans;
uniform mat4 projection;
uniform mat4 view;

void main() 
{
	//gl_Position = projection * view * vTrans * vRotate * vPosition;
	gl_Position = vPosition;

	fragmentColor = vfColor;
	UV = vertexUV;
} 