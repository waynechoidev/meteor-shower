#version 420

in vec2 position;
in float speed;

uniform float deltaTime;
uniform vec2 windowSize;

out vec2 newPosition;

void main()
{
    float expansionRatio = 1.2;

    vec2 newPositionTemp = position + vec2(-speed) * deltaTime;

    vec2 expandedWindowSize = windowSize * expansionRatio;
    vec2 adjustedPosition = mod(newPositionTemp, expandedWindowSize);

    // Wrap around if position is outside the window bounds
    adjustedPosition = mix(adjustedPosition, adjustedPosition - expandedWindowSize, step(windowSize, adjustedPosition));

    newPosition = adjustedPosition;
}
