#version 420

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in float distanceVal[];
in vec3 color[];

out vec2 texCoord;
out vec3 colorValue;
out float distanceValue;

uniform vec2 resolution;

void main() {
    vec4 bottomLeftVertex = gl_in[0].gl_Position;;
    vec4 topRightVertex = bottomLeftVertex + vec4(-1.0 * resolution.y / resolution.x, -1.0, 0.0, 0.0) * distanceVal[0];

    // Bottom Left
    gl_Position = bottomLeftVertex;
    texCoord = vec2(0.0,0.0);
    colorValue = color[0];
    distanceValue = distanceVal[0];
    EmitVertex();

    // Bottom Right
    gl_Position = vec4(topRightVertex.x, bottomLeftVertex.y, 0.0, 1.0);
    texCoord = vec2(1.0,0.0);
    colorValue = color[0];
    distanceValue = distanceVal[0];
    EmitVertex();

    // Top Left
    gl_Position = vec4(bottomLeftVertex.x, topRightVertex.y, 0.0, 1.0);
    texCoord = vec2(0.0,1.0);
    colorValue = color[0];
    distanceValue = distanceVal[0];
    EmitVertex();

    // Top Right
    gl_Position = topRightVertex;
    texCoord = vec2(1.0,1.0);
    colorValue = color[0];
    distanceValue = distanceVal[0];
    EmitVertex();

    EndPrimitive();
}
