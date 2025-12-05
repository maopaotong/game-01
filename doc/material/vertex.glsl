#version 150 core

in vec3 vertex;
in vec2 uv0;
in vec3 normal;

uniform mat4 world_matrix;
uniform mat4 worldviewproj_matrix;
out vec2 fUV;
out vec2 fUV1;
out vec3 fPosition;
out vec3 fNormal;

void main() {

	gl_Position = worldviewproj_matrix * vec4(vertex, 1.0);
	//shader out var.
	vec4 vPosition = world_matrix * vec4(vertex, 1.0);
	fPosition = vPosition.xyz;
	fUV = uv0;
	fNormal = normal;
	float wWidth = 30 * 2 * (128 + 1);
	float wHeight = wWidth * 0.86602540378;

	vec2 tUV1 = vec2(fPosition.x, -fPosition.z);
	tUV1 = vec2(tUV1.x / wWidth, tUV1.y / wHeight);
	fUV1 = tUV1 + vec2(0.5, 0.58);
}
