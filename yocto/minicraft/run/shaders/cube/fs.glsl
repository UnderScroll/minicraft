#version 400
in vec3 position;
in vec3 normal;
in vec2 uv;

in vec4 color_out_vs;

out vec4 color_out;

void main()
{
	color_out = vec4(1, 0, 0, 1);
}