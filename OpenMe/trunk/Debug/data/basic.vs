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
attribute vec3 omePosition;
attribute vec3 omeNormal;
uniform omeMaterial mat;

// outputs
varying vec3 color;
varying vec3 normal;
varying vec3 position;

void main(void)
{	
	gl_Position = gl_ModelViewProjectionMatrix * vec4(omePosition, 1.f);
	
	color = mat.ambiant;
	normal = omeNormal;
	position = omePosition;
}
