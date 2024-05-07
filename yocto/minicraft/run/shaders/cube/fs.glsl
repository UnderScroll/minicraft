#version 400

in vec4 color;
in vec3 position;
in vec3 normal;
in vec2 uv;

out vec4 color_out;

uniform vec3 toLight;

void main()
{
	color_out = vec4(color.rgb * max(0.03, dot(toLight, normal)), color.a);

	color_out.r = clamp(color_out.r, 0, 1);
	color_out.g = clamp(color_out.g, 0, 1);
	color_out.b = clamp(color_out.b, 0, 1);
}