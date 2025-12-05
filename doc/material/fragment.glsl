#version 150 core

in vec3 fPosition;
in vec2 fUV;
in vec3 fNormal;
out vec4 outFColor;  //

uniform sampler2D texTerr;
uniform sampler2D texOcean;
uniform sampler2D texShore;
uniform sampler2D texLand;
uniform sampler2D texMountain;

const float HEIGHT_SCALE = 100;

bool isTypeEquals(float a, float b) {
    return abs(a - b) < 1e-6;
}

bool isOcean(float height) {
    return height > -0.001 && height <= 49.5;
}

bool isShore(float height) {
    return height > 49.5 && height <= 50.5;
}

bool isLand(float height) {

    return height > 50.5 && height <= 51.5;
}

bool isMountain(float height) {
    return height > 51.5 && height <= 100;
}

void main() {

//outFragColor = vec4(1.0, 0.0, 1.0, 1.0); // 亮品红
//gl_FragColor = vec4(1.0,0.0,0.0,1.0);
    vec4 color;
    vec3 normal = fNormal;
    vec4 terr = texture(texTerr, fUV);

    //float noise = fract(sin(dot(fUV, vec2(1.234, 57.890123))) * 321.564);

    float height = fPosition.y;// + (noise - 0.5) * 0.2;

    if(false && abs(height - 35) < 2.0) {
        outFColor = vec4(1.0, 0.0, 1.0, 1.0); // 品红：靠近海洋/沙滩边界
    } else {

        if(isOcean(height)) {
            color = texture(texOcean, fUV);
        //normal = vec3(0,1,0);
        } else if(isShore(height)) {
            color = texture(texShore, fUV);
        //normal = vec3(0,1,0);
        } else if(isLand(height)) {
            color = texture(texLand, fUV);
        } else if(isMountain(height)) {
            color = texture(texMountain, fUV);
        }

//outFColor = vec4(fPosition * 0.1 + 0.5, 1.0); // 调试用
//float NdotL = fNormal.y; // 假设光从上方来（方向 = (0,1,0)）
//outFColor = outFColor * clamp(NdotL * 0.8 + 0.2, 0.0, 1.0);
//outFColor = vec4(fNormal*100, 1);
//outFColor = vec4(normalize(fNormal)*0.5+0.5, 1);

        vec3 N = normalize(normal);           // ← 关键！
        vec3 L = normalize(vec3(0.75, 1.0, 0.0)); // 光从右上方来
        float ambient = 0.68;
        float diffuse = 1.2 * max(dot(N, L), 0.0);
        float diff = ambient + diffuse;

        //float shade_factor = 0.58 + 1.60 * max(0., dot(vNormal, normalize(light)));

        outFColor = vec4(color.rgb * (diff + 0.2), 1.0); // +0.2 防全黑
    }
    //outFColor = vec4(vec3(height / 100.0), 1.0); // 假设 maxHeight 是你期望的最大高度
    //outFColor = vec4(vec3(noise), 1.0);
}