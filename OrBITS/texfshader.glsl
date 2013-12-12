#version 150

in vec2 UV;
uniform sampler2D myTextureSampler;

out vec4 fColor;

void
main()
{
	// Get texture color data
	fColor = texture( myTextureSampler, UV.xy ).rgba;
}
