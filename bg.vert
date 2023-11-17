#version 420

in vec2 pos;
in vec2 tex;

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(pos, 0.1, 1.0);
	TexCoords = tex;
}