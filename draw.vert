#version 420

in vec2 pos;
in float dis;
in vec3 col;

uniform mat4 projection;

out float distanceVal;
out vec3 color;

void main()
{
    gl_Position = projection * vec4(pos, 1.0, 1.0);
    distanceVal = dis;
    color = col;
}