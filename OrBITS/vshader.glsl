#version 150 

in  vec4 vPosition;
in vec4 vfColor;

out vec4 fragmentColor;

uniform mat4 vRotate;
uniform mat4 vTrans;
uniform mat4 projection;
uniform mat4 view;

void main() 
{
    //gl_Position = projection*model_view*vPosition/vPosition.w;
	//gl_Position = projection*view*vPosition;
	gl_Position = projection * view * vTrans * vRotate * vPosition;
	/*gl_Position = view * gl_Position;
	gl_Position = vTrans * gl_Position;
	gl_Position = projection * gl_Position;*/

	fragmentColor = vfColor;
} 