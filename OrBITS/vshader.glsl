#version 150

in vec4 vPosition;
in vec4 vfColor;

out vec4 fragmentColor;

uniform mat4 vRotate;
uniform mat4 vTrans;
uniform mat4 projection;
uniform mat4 view;

void
main()
{
	// Get rotation, translation, camera (view + projection)
	gl_Position = projection * view * vTrans * vRotate * vPosition;
	
	// Pass color
	fragmentColor = vfColor;
}