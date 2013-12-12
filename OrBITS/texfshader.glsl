#version 150

in vec2 UV;
out vec3 color;
uniform sampler2D myTextureSampler;

out vec4 fColor;

void
main()
{
    //fColor = fragmentColor;
	//fColor = vec4(1.0, 0.0, 0.0, 1.0);
	//fColor = vec4(UV.x, UV.y, UV.y, 1.0);

	//UV = vec3(1.0, 0.0, 0.0);

	fColor = texture( myTextureSampler, UV.xy).rgba;
}
