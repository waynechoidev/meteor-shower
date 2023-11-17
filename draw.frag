#version 420

in vec3 colorValue;
in vec2 texCoord;
in float distanceValue;

uniform float currentTime;
uniform vec2 resolution;

out vec4 color;

float circle(vec2 st, vec2 center, float radius) {
    float distance = length(st - center);

    if (distance > radius*0.9) return 0.0;

    return 1.0;
}

float glow(vec2 st, vec2 center, float radius) {
    float distance = length(st - center);
    return 1.0 - smoothstep(radius * 0.5, radius * 3.0, distance);
}

float tail(vec2 st, vec2 center, float radius) {    
    if (st.x + st.y < center.x + center.y) return 0.0;
    return smoothstep(radius * 0.5, 0.0, abs(st.y - st.x) * (st.x + radius * 10.0));
}

void main() {
	vec2 st = texCoord;
    st.x = 1.0 - st.x;
    st.y = 1.0 - st.y;

    vec2 center = vec2(0.05, 0.05);
    float radius = 0.02;

    float pct = circle(st, center, radius) + glow(st, center, radius) + tail(st, center, radius);
    float brightness = 0.7 + 0.3 * sin(currentTime * distanceValue * 10.0);

    color = vec4(pct * colorValue * brightness, 1.0);

    if (dot(color.rgb, vec3(1.0)) - 0.1 < 0.0) {
    discard;
    }

}
