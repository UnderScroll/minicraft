#version 400

uniform mat4 mvp;
uniform vec4 cube_color;

layout(location=0) in vec3 vs_position_in;
layout(location=1) in vec3 vs_normal_in;
layout(location=2) in vec2 vs_uv_in;
layout(location=3) in float vs_type_in;

out vec4 color;
out vec3 position;
out vec3 normal;
out vec2 uv;

vec4 colors[5] = vec4[](
	vec4(0.0, 0.0, 0.0, 0),
	vec4(0.0, 1.0, 0.0, 1),
	vec4(0.2, 0.1, 0.0, 1),
	vec4(0.4, 0.5, 0.5, 1),
	vec4(0.1, 0.2, 0.9, 0.5)
);

void main()
{
	uint type_index = uint(vs_type_in);
	vec4 vecIn = vec4(vs_position_in,1.0);
	gl_Position = mvp * vecIn;

	position = vs_position_in;
	normal = vs_normal_in;
	uv = vs_uv_in;

	color = colors[type_index];
}