#version 150 core

in vec4 vertex;
in vec2 uv0;

uniform mat4 worldviewproj_matrix;

out vec2 fragUV;

void main()                    
{
	gl_Position = worldviewproj_matrix * vertex;
	fragUV = uv0;
	
}
