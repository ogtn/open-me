#version 120

// inputs
attribute vec3 omeVertexPosition;
attribute vec4 omeColor;
uniform mat4 omeMeshMatrix;
uniform mat4 omeProjection;
uniform mat4 omeView;

// outputs
varying vec4 color;

void main(void)
{
	gl_Position = omeProjection * omeView * omeMeshMatrix * vec4(omeVertexPosition, 1.f);
	color = omeColor;
}
