#version 150

in vec2 UV;
out vec3 color;
uniform sampler2D myTextureSampler;

in vec4 fragmentColor;
out vec4 fColor;

void
main()
{
    fColor = fragmentColor;
	//fColor = vec4(1.0, 0.0, 0.0, 1.0);

	//color = texture( myTextureSampler, UV).rgb;
}
