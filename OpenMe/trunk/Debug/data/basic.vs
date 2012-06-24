#version 120

varying vec3 color;
uniform float testUniform = 1;

void main(void)
{
	gl_Position = ftransform();
	color = gl_Normal * testUniform;
}
