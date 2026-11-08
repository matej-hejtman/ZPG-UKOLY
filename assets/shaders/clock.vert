// Author: Matt Hejtman, login: HEJ0094
#version 330 core

// Clock in one draw call: the hands are recognized by their vertex color and
// rotated here around the center of the dial (origin, axis z) by the time angles.

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);
uniform float hourAngle;     // radians
uniform float minuteAngle;
uniform float secondAngle;

out vec3 vertexColor;

const vec3 SECOND_COLOR = vec3(1.0, 0.0, 0.0);
const vec3 MINUTE_COLOR = vec3(0.0, 1.0, 0.0);
const vec3 HOUR_COLOR = vec3(0.0, 0.0, 1.0);
const float TOLERANCE = 0.01;

// Rotation of a point in the xy plane around the origin (axis z)
vec3 rotateZ(vec3 p, float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return vec3(c * p.x - s * p.y, s * p.x + c * p.y, p.z);
}

void main()
{
    vec3 p = position;
    if (distance(color, SECOND_COLOR) < TOLERANCE)
        p = rotateZ(p, secondAngle);
    else if (distance(color, MINUTE_COLOR) < TOLERANCE)
        p = rotateZ(p, minuteAngle);
    else if (distance(color, HOUR_COLOR) < TOLERANCE)
        p = rotateZ(p, hourAngle);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(p, 1.0);
    vertexColor = color;
}
