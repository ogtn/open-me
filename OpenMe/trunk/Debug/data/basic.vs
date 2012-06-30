#version 120

// types
struct omeMaterial
{
	vec3 ambiant;
	//sampler2D diffuseTexture;
	//vec3 specularColor;
	//vec3 emissiveColor;
	//float shininess;
};

// inputs
attribute vec3 omeVertexPosition;
attribute vec3 omeNormal;
uniform omeMaterial mat;
uniform vec3 omeMeshPosition;
uniform mat4 omeProjection;
uniform mat4 omeModelview;
uniform vec3 omeCameraPosition;

// outputs
varying vec3 color;
varying vec3 normal;
varying vec3 position;

void main(void)
{
	vec3 realPosition = omeVertexPosition + omeMeshPosition;
	gl_Position = omeProjection * omeModelview * vec4(realPosition, 1.f);
	
	color = mat.ambiant;
	normal = omeNormal;
	position = omeCameraPosition - realPosition;
}
