#version 150 core

in vec3 fPosition;
in vec2 fUV;
in vec2 fUV1;
in vec3 fNormal;
out vec4 outFColor;  //

uniform sampler2D tex_t;//0
//
uniform sampler2D tex_o;//1
uniform sampler2D tex_s;//2
uniform sampler2D tex_p;//3
uniform sampler2D tex_h;//4
uniform sampler2D tex_m;//5
uniform sampler2D tex_f;//6

const float HEIGHT_SCALE = 100;
//
const float TT_OCEAN = 1;
const float TT_SHORE = 2;
const float TT_PLAIN = 3;
const float TT_HILL = 4;
const float TT_MOUNTAIN = 5;
const float TT_FROZEN = 6;
const float TT_DESERT = 7;
const float TT_LAKE = 8;
const float TT_RIVER = 9;
const float TT_UNKNOW = 0;

bool isType(float a, float b) {
    return abs(a - b) < 1e-6;
}

bool isOcean(int type) {
    //return height > -0.001 && height <= 49.5;
    return type == TT_OCEAN;
}

bool isShore(int type) {
    return type == TT_SHORE;
}

bool isPlain(int type) {

    return type == TT_PLAIN;
}

bool isMountain(int type) {
    return type == TT_MOUNTAIN;
}

bool isHill(int type) {
    return type == TT_HILL;
}

bool isFrozen(int type) {
    return type == TT_FROZEN;
}


void main() {

//outFragColor = vec4(1.0, 0.0, 1.0, 1.0); // 亮品红
//gl_FragColor = vec4(1.0,0.0,0.0,1.0);
    vec4 color;
    vec3 normal = fNormal;
    vec4 terr = texture(tex_t, fUV1);
    //outFColor = terr;

    int type = int(terr.r * 255);
    float ftype = terr.r;
    //float noise = fract(sin(dot(fUV, vec2(1.234, 57.890123))) * 321.564);

    float height = fPosition.y;// + (noise - 0.5) * 0.2;

    if(false) {
        outFColor = vec4(ftype, 1, 0, 1);
    }
    if(false) {
        outFColor = vec4(fUV1, 0.0, 1.0);
    }

    if(false) {
        if(ftype < 0) {
            outFColor = vec4(1.0, 1.0, 0, 1.0); //R
        } else if(ftype == 0) {
            outFColor = vec4(0, 1.0, 1, 1.0); //R    
        } else if(ftype > 0 && ftype < 1) {
            outFColor = vec4(1.0, 0.0, 0, 1.0); //R
        } else if(ftype >= 1 && ftype < 1.1) {
            outFColor = vec4(0, 1.0, 0, 1.0); //G
        } else {
            outFColor = vec4(0, 0, 1, 1);//B
        }

    }

    if(false) {
        if(type <= 0) {
            outFColor = vec4(1.0, 1.0, 0, 1.0); //R
        } else if(type >= 0 && type <= 4) {
            outFColor = vec4(0, 1.0, 1, 1.0); //R    
        } else if(type >= 5 && type <= 10) {
            outFColor = vec4(1.0, 0.0, 0, 1.0); //R
        } else if(type >= 11 && type <= 100) {
            outFColor = vec4(0, 1.0, 0, 1.0); //G
        } else {
            outFColor = vec4(0, 0, 1, 1);//B
        }

    }

    if(true) {
        
        if(isOcean(type)) {            
            color = texture(tex_o, fUV);
        } else if(isShore(type)) {                        
            color = texture(tex_s, fUV);
        } else if(isPlain(type)) {            
            color = texture(tex_p, fUV);
        } else if(isHill(type)) {            
            color = texture(tex_h, fUV);
        } else if(isMountain(type)) {            
            color = texture(tex_m, fUV);
        } else if(isFrozen(type)) {            
            color = texture(tex_f, fUV);
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
        //color = terr;
        outFColor = vec4(color.rgb * (diff + 0.2), 1.0); // +0.2 防全黑
        //outFColor = terr;
    }
    //outFColor = vec4(vec3(height / 100.0), 1.0); // 假设 maxHeight 是你期望的最大高度
    //outFColor = vec4(vec3(noise), 1.0);

}