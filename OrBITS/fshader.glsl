#version 150

in vec4 fragmentColor;
out vec4 fColor;

void
main()
{
    fColor = fragmentColor;
	//fColor = vec4(1.0, 0.0, 0.0, 1.0);
}
