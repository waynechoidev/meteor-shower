#version 420

in vec2 TexCoords;

out vec4 color;

uniform sampler2D theTexture;

void main()
{
	color = texture(theTexture, TexCoords);
	//color = vec4(1.0);
	if (dot(color.rgb, vec3(1.0)) - 0.1 < 0.0) {
    discard;
    }
}