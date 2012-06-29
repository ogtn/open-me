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
uniform float testUniform = 0.f;
uniform omeMaterial mat;

// outputs
varying vec3 color;

void main(void)
{
	vec4 newPos = vec4(omePosition, 1.f);
	newPos.x += sin((gl_Vertex.z / 4.f) + testUniform) * 4;
	gl_Position = gl_ModelViewProjectionMatrix * newPos;
	color = /*mat.ambiant.rgb * 0.25 +*/ omeNormal * 0.75;
}
