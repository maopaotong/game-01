#version 150 core

in vec3 fPosition;
in vec2 fUV;
in vec3 fNormal;
out vec4 outFColor;  // ← 显式声明

uniform sampler2D texTiles;
uniform sampler2D texOcean;
uniform sampler2D texShore;
uniform sampler2D texLand;
uniform sampler2D texMountain;

const float HEIGHT_SCALE = 100;

bool fEquals(float a, float b) {
    return abs(a - b) < 0.001;
}

bool isOcean(float height){    
    return height > -1 && height < 45;
}

bool isShore(float height){    
    return height > 1 && height < 51;
}

bool isLand(float height){

    return height > 50 && height < 76;
}

bool isMountain(float height){
    return height > 75;
}


void main() {

//outFragColor = vec4(1.0, 0.0, 1.0, 1.0); // 亮品红
//gl_FragColor = vec4(1.0,0.0,0.0,1.0);
    vec4 color;
    if(isOcean(fPosition.y)){
        color = texture(texOcean, fUV);
    }else if(isShore(fPosition.y)){
        color = texture(texShore, fUV);        
    }else if(isLand(fPosition.y)){
        color = texture(texLand, fUV);
    }else if(isMountain(fPosition.y)){
        color = texture(texMountain, fUV);
    }

//outFColor = vec4(fPosition * 0.1 + 0.5, 1.0); // 调试用
//float NdotL = fNormal.y; // 假设光从上方来（方向 = (0,1,0)）
//outFColor = outFColor * clamp(NdotL * 0.8 + 0.2, 0.0, 1.0);
//outFColor = vec4(fNormal*100, 1);
//outFColor = vec4(normalize(fNormal)*0.5+0.5, 1);

    vec3 N = normalize(fNormal);           // ← 关键！
    vec3 L = normalize(vec3(0.5, 1.0, 0.5)); // 光从右上方来
    float diff = max(dot(N, L), 0.0);

    outFColor = vec4(color.rgb * (diff + 0.2), 1.0); // +0.2 防全黑
}