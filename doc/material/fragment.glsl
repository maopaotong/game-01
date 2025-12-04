#version 150 core

in vec2 fragUV;
out vec4 outFragColor;  // ← 显式声明

uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{

//outFragColor = vec4(1.0, 0.0, 1.0, 1.0); // 亮品红
//gl_FragColor = vec4(1.0,0.0,0.0,1.0);
outFragColor = texture(tex0, fragUV);
outFragColor = texture(tex1, fragUV);

}