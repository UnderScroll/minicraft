#version 400

in vec4 color;
in vec3 position;
in vec3 normal;
in vec2 uv;

out vec4 color_out;

void main()
{
	color_out = vec4(normal, 1);
}