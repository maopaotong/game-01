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

const float sqrt3 = sqrt(3.0);
const float wWidth = 30 * 2 * (128 + 1);
const float wHeight = wWidth * sqrt3 / 2.0;
const vec2 wOffset = vec2(wWidth / 2.0, wHeight / 2.0);

void main() {

	gl_Position = worldviewproj_matrix * vec4(vertex, 1.0);
	//shader out var.
	vec4 vPosition = world_matrix * vec4(vertex, 1.0);
	fPosition = vPosition.xyz;
	fUV = uv0;
	fNormal = normal;
	vec2 posIn2D = vec2(fPosition.x, -fPosition.z) + wOffset;

	fUV1 = vec2(posIn2D.x / wWidth, posIn2D.y / wHeight) + vec2(0, 0.078);//

}
