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
attribute vec2 omeTextCoord0;
uniform omeMaterial mat;
uniform mat4 omeMeshMatrix;
uniform mat4 omeProjection;
uniform mat4 omeModelview;
uniform vec3 omeCameraPosition;

// outputs
varying vec3 color;
varying vec3 normal;
varying vec3 position;
varying vec2 texCoord;

void main(void)
{
	gl_Position = omeProjection * omeModelview * omeMeshMatrix * vec4(omeVertexPosition, 1.f);
	
	//color = mat.ambiant;
	//normal = omeNormal;
	//position = omeCameraPosition - realPosition;
	texCoord = omeTextCoord0;
}
