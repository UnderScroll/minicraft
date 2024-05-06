#version 400

uniform mat4 mvp;
uniform vec4 cube_color;

layout(location=0) in vec3 vs_position_in;
layout(location=1) in vec3 vs_normal_in;
layout(location=2) in vec2 vs_uv_in;

out vec4 color;
out vec3 position;
out vec3 normal;
out vec2 uv;

void main()
{
	vec4 vecIn = vec4(vs_position_in,1.0);
	gl_Position = mvp * vecIn;

	color = cube_color;
	position = vs_position_in;
	normal = vs_normal_in;
	uv = vs_uv_in;
}