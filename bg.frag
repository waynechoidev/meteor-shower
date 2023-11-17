#version 420

in vec2 TexCoords;

out vec4 color;

uniform sampler2D theTexture;

void main()
{
	color = texture(theTexture, TexCoords);
	//color = vec4(1.0);
}