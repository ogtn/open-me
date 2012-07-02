#version 120

// inputs
attribute vec3 omeVertexPosition;
uniform vec3 omeMeshPosition;
uniform mat4 omeProjection;
uniform mat4 omeModelview;
uniform vec3 omeCameraPosition;

void main(void)
{
	vec3 realPosition = omeVertexPosition + omeMeshPosition;
	gl_Position = omeProjection * omeModelview * vec4(realPosition, 1.f);
}
