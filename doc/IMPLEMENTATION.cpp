#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// 加载字体文件（如 Roboto.ttf）
unsigned char* ttf_buffer = new unsigned char[1 << 20];
FILE* fontFile = fopen("Roboto.ttf", "rb");
fread(ttf_buffer, 1, 1 << 20, fontFile);
fclose(fontFile);

stbtt_fontinfo font;
stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));

const char* text = "Building A";
int width = 256, height = 64;
unsigned char* pixels = new unsigned char[width * height];

// 渲染黑白文字（简化版）
stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0f, pixels, width, height, 32, 95, cdata);

// 转为 RGBA（白色文字 + alpha）
Ogre::uint8* rgba = new Ogre::uint8[width * height * 4];
for (int i = 0; i < width * height; ++i) {
    rgba[i*4 + 0] = 255; // R
    rgba[i*4 + 1] = 255; // G
    rgba[i*4 + 2] = 255; // B
    rgba[i*4 + 3] = pixels[i]; // A
}

// 上传到 GPU 纹理
Ogre::TextureGpu* tex = renderSystem->getTextureGpuManager()->createOrRetrieveTexture(
    "DynamicTextTex",
    Ogre::GpuPageOutStrategy::Always,
    Ogre::TextureFlags::AutomaticBatching,
    Ogre::TextureTypes::Type2D,
    nullptr
);
tex->setResolution(width, height);
tex->setPixelFormat(Ogre::PFG_RGBA8_UNORM);
tex->setStaticTextures(rgba, width * height * 4);

delete[] rgba;
delete[] pixels;
delete[] ttf_buffer;