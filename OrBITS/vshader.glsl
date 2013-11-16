#version 150 

in  vec4 vPosition;
in vec4 vfColor;

out vec4 fragmentColor;

uniform mat4 vRotate;
uniform mat4 vTrans;
uniform mat4 model_view;
uniform mat4 projection;

void main() 
{
    //gl_Position = projection*model_view*vPosition/vPosition.w;
	gl_Position = vRotate * vPosition;
	gl_Position = vTrans * gl_Position;

	fragmentColor = vfColor;
} 