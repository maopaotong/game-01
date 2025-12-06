#version 150 core

in vec3 fPosition;
in vec2 fUV;
in vec2 fUV1;
in vec3 fNormal;
//out color
out vec4 outColor;  //

//tiles meta data , e.g. r is type of tile.
uniform sampler2D tex_t;//0
//
uniform sampler2D tex_o;//1 ocean
uniform sampler2D tex_s;//2 shore
uniform sampler2D tex_p;//3 plain
uniform sampler2D tex_h;//4 hill
uniform sampler2D tex_m;//5 mountain
uniform sampler2D tex_f;//6 frozen
uniform sampler2D tex_b;//7 beach

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

const float sqrt3 = sqrt(3.0);

// 
ivec2 getCellKey(vec2 pos2D, float rad) {
    float R = (2.0 / sqrt3) * rad;
    // 
    // Step 1: (q, r)
    float q = ((sqrt3 / 3.0) * pos2D.x - (1.0 / 3.0) * pos2D.y) / R;
    float r = ((2.0 / 3.0) * pos2D.y) / R; 

    // Cube
    float x = q;
    float z = r;
    float y = -x - z;
    // 
    int rx = int(round(x));
    int ry = int(round(y));
    int rz = int(round(z));

    float dx = abs(rx - x);
    float dy = abs(ry - y);
    float dz = abs(rz - z);

    if(dx > dy && dx > dz) {
        rx = -ry - rz;
    } else if(dy > dz) {
        ry = -rx - rz;
    } else {
        rz = -rx - ry;
    }

    // Fix rounding to ensure q + r + s == 0
    int row = rz;
    int col = rx + (row - (row & 1)) / 2;
    return ivec2(col, row);
}

bool isOcean(int type) {
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

vec2 getOrigin2D(ivec2 cKey, float rad) {
    float centerX = cKey.x * 2 * rad + (cKey.y % 2 == 0 ? 0 : rad);
    float centerY = cKey.y * rad * sqrt3;
    return vec2(centerX, centerY);
}

float distanceToCell(vec2 posIn2D, float rad) {
    ivec2 cKey = getCellKey(posIn2D, rad);
    vec2 cIn2D = getOrigin2D(cKey, rad);

    return distance(cIn2D, posIn2D) / rad;
}

vec2 rotateClockwise60(vec2 p) {
    const float c = 0.5;               // cos(60°)
    const float s = sqrt3 / 2.0;   // sin(60°)
    return vec2(c * p.x + s * p.y, -s * p.x + c * p.y);
}
//
float distanceToEdge(vec2 posIn2D, float rad) {
    ivec2 cKey = getCellKey(posIn2D, rad);
    vec2 cIn2D = getOrigin2D(cKey, rad);
    vec2 p = posIn2D - cIn2D;
    p = abs(p);
    if(p.y > (1.0 / sqrt3) * p.x) {
        p = rotateClockwise60(p);
    }
    return p.x - rad; // < 0 inner, > 0 outer.
}
//
const bool show_edge = true;
//
const float radInUv = 0.5 / 129.0;//rad / width of tiles 
const float MOUNTAIN_FOOT = 52.50;
const float HILL_FOOT = 51.50;
const float PLAIN_FOOT = 42.05;
const float BEACH_FOOT = 41.05;
const float SHORE_FOOT = 40.50;

void main() {

    vec4 color;
    vec3 normal = fNormal;

    // ivec2 cKey = getCellKey(fUV1, radInUv);//    
    // vec2 cUv = getOrigin2D(cKey, radInUv);

    //cUv = fUV1;
    vec4 terr = texture(tex_t, fUV1);
    //outColor = terr;

    int type = int(terr.r * 255);

    //

    //distance to cell centre 0..1..(1 * 2 / sqrt3):centre..rad..R;
    float distance = terr.g * 255.0 / 100.0;

    if(true) {

        if(isFrozen(type)) {

            color = texture(tex_f, fUV);

        } else if(isMountain(type)) {

            if(fPosition.y > MOUNTAIN_FOOT) { // higher
                color = texture(tex_m, fUV);
            } else {    //lower
                color = texture(tex_h, fUV);//
            }

        } else if(isHill(type)) {
            if(fPosition.y > HILL_FOOT) { // higher
                color = texture(tex_h, fUV);
            } else {    //lower
                color = texture(tex_p, fUV);//
            }

        } else if(isPlain(type)) {

            if(fPosition.y > PLAIN_FOOT) { // higher
                color = texture(tex_p, fUV);
            } else if(fPosition.y > BEACH_FOOT) {// 
                color = texture(tex_b, fUV);
            } else {    //
                color = texture(tex_s, fUV);// as shore
            }

        } else if(isShore(type)) {
            if(fPosition.y > PLAIN_FOOT) {// higher
                color = texture(tex_p, fUV); //as plain
            } else if(fPosition.y > BEACH_FOOT) {
                color = texture(tex_b, fUV); //as beach
            } else if(fPosition.y > SHORE_FOOT) {
                color = texture(tex_s, fUV); //shore itself
            } else {
                color = texture(tex_s, fUV); //shore itself
            }
        } else if(isOcean(type)) {
            if(fPosition.y > SHORE_FOOT) {
                color = texture(tex_s, fUV);
            } else {
                color = texture(tex_o, fUV);
            }
        }

        float ambient = 0.68;
        if(show_edge) {
            if(distance > 0.8) {
                vec2 posIn2D = vec2(fPosition.x, -fPosition.z) * (1.0 / 30.0) + vec2(-1.0, sqrt3 / 2.0);//offset and normal rad to 1.
                float distance2 = distanceToEdge(posIn2D, 1.0);//
                //distance2 = abs(distance2);

                if(-0.01 < distance2 && distance2 < 0.01) {
                    //ambient *= 8;
                    color = mix(color, vec4(1, 1, 1, 1), 0.5);
                }

            }
        }

        vec3 N = normalize(normal);           //
        vec3 L = normalize(vec3(0.75, 1.0, 0.0)); // light direction.
        float diffuse = 1.2 * max(dot(N, L), 0.0);
        float diff = ambient + diffuse;

        outColor = vec4(color.rgb * (diff + 0.2), 1.0); // +0.2 
        //outColor = terr;
    }//end if true

}//end of main()

/////////////////////////////////////////////////////////////////////////////
void test(vec4 terr, int type) {
    float ftype = terr.r;
    //float noise = fract(sin(dot(fUV, vec2(1.234, 57.890123))) * 321.564);

    float height = fPosition.y;// + (noise - 0.5) * 0.2;

    if(false) {
        outColor = vec4(ftype, 1, 0, 1);
    }
    if(false) {
        outColor = vec4(fUV1, 0.0, 1.0);
    }

    if(false) {
        if(ftype < 0) {
            outColor = vec4(1.0, 1.0, 0, 1.0); //R
        } else if(ftype == 0) {
            outColor = vec4(0, 1.0, 1, 1.0); //R    
        } else if(ftype > 0 && ftype < 1) {
            outColor = vec4(1.0, 0.0, 0, 1.0); //R
        } else if(ftype >= 1 && ftype < 1.1) {
            outColor = vec4(0, 1.0, 0, 1.0); //G
        } else {
            outColor = vec4(0, 0, 1, 1);//B
        }

    }

    if(false) {
        if(type <= 0) {
            outColor = vec4(1.0, 1.0, 0, 1.0); //R
        } else if(type >= 0 && type <= 4) {
            outColor = vec4(0, 1.0, 1, 1.0); //R    
        } else if(type >= 5 && type <= 10) {
            outColor = vec4(1.0, 0.0, 0, 1.0); //R
        } else if(type >= 11 && type <= 100) {
            outColor = vec4(0, 1.0, 0, 1.0); //G
        } else {
            outColor = vec4(0, 0, 1, 1);//B
        }

    }

}//end of test()

/////////////////////////////////////////////////////////////////////////////