#version 150

in vec4 vPosition;
in vec4 vfColor;

out vec4 fragmentColor;

uniform mat4 vRotate;
uniform mat4 vTrans;
uniform mat4 vScale;
uniform mat4 vSkew;
uniform mat4 projection;
uniform mat4 view;

void
main()
{
	//mat4 vSkew = mat4(1.0);

	// Get rotation, translation, camera (view + projection)
	gl_Position = projection * view * vTrans * vSkew * vScale * vRotate * vPosition;

	// Pass color
	fragmentColor = vfColor;
}