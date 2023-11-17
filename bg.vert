#version 420

in vec2 pos;
in vec2 tex;

uniform float zIndex;

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(pos, zIndex, 1.0);
	TexCoords = tex;
}