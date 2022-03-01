#version 410
/** @note PA2: In this shader you should change the first attribute to be 3d instead of 2d.
 * Then (in part 3) you shall add a uniform MVP matrix and use it in the standard way.
 */

// ins (inputs)
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColors;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
// outs
out vec4 color;

void main()
{
    gl_Position =  P*V*M * vec4(vertexPosition, 1);
    color = vec4(vertexColors, 1);
}
