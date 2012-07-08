#version 120

// types
struct omeMaterial
{
	vec3 ambiant;
};

// inputs
attribute vec3 omeVertexPosition;
attribute vec3 omeNormal;
uniform omeMaterial mat;
uniform mat4 omeMeshMatrix;

uniform mat4 omeProjection;
uniform mat4 omeView;
uniform vec3 omeCameraPosition;

// outputs
varying vec3 normal;
varying vec3 position;
varying vec3 camPos;

void main(void)
{
	mat4 modelview = omeView * omeMeshMatrix;
	
	gl_Position = omeProjection * modelview * vec4(omeVertexPosition, 1.f);
	
	position = omeVertexPosition;
	camPos = omeCameraPosition;
	normal = omeNormal;
}
