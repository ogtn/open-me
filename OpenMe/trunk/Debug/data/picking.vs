#version 120

// inputs
attribute vec3 omeVertexPosition;
uniform mat4 omeMeshMatrix;
uniform mat4 omeProjection;
uniform mat4 omeModelview;
uniform vec3 omeCameraPosition;

void main(void)
{
	gl_Position = omeProjection * omeModelview * omeMeshMatrix * vec4(omeVertexPosition, 1.f);
}
