#pragma once

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include <OgreSceneManager.h>
#include <OgreSharedPtr.h>
#include <stb_image_write.h>
#include <fg/State.h>
#include <OgreShaderSubRenderState.h>
#include <OgreShaderRenderState.h>
#include <OgreShaderGenerator.h>

using namespace Ogre;

//
// SubRenderStateFactory::~SubRenderStateFactory()
//{
//    OgreAssert(mSubRenderStateList.empty(), "Sub render states still exists");
//}
// To fix the above unexpected exception when closing the app context:
//- Do not delete the pointer by shared pointer.
//- We need to detroy it before app closing.

class GameTerrain : public State
{
    const String FFP_Transform = "FFP_Transform";
    const float flatHight = 0.0f;

private:
    long terrainX = 1;
    long terrainY = 1;
    int terrainSize = 513;
    float worldSize = 2000.0f;
    Ogre::TerrainGroup *terrainGroup = nullptr;
    Ogre::TerrainGlobalOptions *options = nullptr;

    TerrainMaterialGeneratorA *defaultTMG = nullptr;

public:
    virtual ~GameTerrain()
    {
       
        if (terrainGroup)
        {
            delete terrainGroup;
        }
        if (options)
        {
            delete options;
        }

        Ogre::RTShader::SubRenderStateFactory *fac = Ogre::RTShader::ShaderGenerator::getSingleton().getSubRenderStateFactory(FFP_Transform);
        fac->destroyAllInstances();
    }

public:
    void load(RenderSystem *renderSys, Ogre::SceneManager *sceMgr, Light *light)
    {
        // terrain group
        terrainGroup = new Ogre::TerrainGroup(
            sceMgr,
            Ogre::Terrain::ALIGN_X_Z,
            terrainSize,
            worldSize);
        terrainGroup->setOrigin(Ogre::Vector3::ZERO);

        // Config =======================================
        options = new Ogre::TerrainGlobalOptions();
        options->setMaxPixelError(8);
        options->setCompositeMapDistance(3000);

        defaultTMG = new TerrainMaterialGeneratorA();
        TerrainMaterialGeneratorPtr defaultTMGPtr(defaultTMG, [](TerrainMaterialGenerator *ptr) {
            delete ptr;
        });
        options->setDefaultMaterialGenerator(defaultTMGPtr);

        TerrainMaterialGeneratorA::SM2Profile *matProfile = static_cast<TerrainMaterialGeneratorA::SM2Profile *>(
            defaultTMG->getActiveProfile());

        // Disable the lightmap for OpenGL ES 2.0. The minimum number of samplers allowed is 8(as opposed to 16 on
        // desktop). Otherwise we will run over the limit by just one. The minimum was raised to 16 in GL ES 3.0.
        if (renderSys->getCapabilities()->getNumTextureUnits() < 9)
        {
            matProfile->setLightmapEnabled(false);
        }

        // Disable steep parallax by default
        matProfile->setLayerParallaxOcclusionMappingEnabled(false);

        options->setLightMapDirection(light->getDerivedDirection());
        options->setCompositeMapAmbient(sceMgr->getAmbientLight());
        options->setCompositeMapDiffuse(light->getDiffuseColour());

        Ogre::Terrain::ImportData &defaultimp = terrainGroup->getDefaultImportSettings();
        defaultimp.inputScale = 600;
        defaultimp.minBatchSize = 33;
        defaultimp.maxBatchSize = 65;

        Ogre::Image combined;
        combined.loadTwoImagesAsRGBA("Ground23_col.jpg", "Ground23_spec.png", "General");
        TextureManager::getSingleton().loadImage("Ground23_diffspec", "General", combined);

        defaultimp.layerList.resize(3);
        defaultimp.layerList[0].worldSize = 200;
        defaultimp.layerList[0].textureNames.push_back("Ground37_diffspec.dds");
        defaultimp.layerList[0].textureNames.push_back("Ground37_normheight.dds");
        defaultimp.layerList[1].worldSize = 200;
        defaultimp.layerList[1].textureNames.push_back("Ground23_diffspec"); // loaded from memory
        defaultimp.layerList[1].textureNames.push_back("Ground23_normheight.dds");
        defaultimp.layerList[2].worldSize = 400;
        defaultimp.layerList[2].textureNames.push_back("Rock20_diffspec.dds");
        defaultimp.layerList[2].textureNames.push_back("Rock20_normheight.dds");

        // Define =====================================
        for (long x = 0; x < terrainX; ++x)
        {
            for (long y = 0; y < terrainY; ++y)
            {
                terrainGroup->defineTerrain(x, y, flatHight);
            }
        }
        // load
        terrainGroup->loadAllTerrains(true);
        // material
        // Ogre::Terrain::ImportData &data =
        terrainGroup->getDefaultImportSettings();

        // blend

        // free
        terrainGroup->freeTemporaryResources();
    }

    void genereatePng(const char *filename, int width, int height)
    {
        // 创建一个宽度x高度大小的灰度图像数据容器
        std::vector<uint8_t> image(width * height);

        // 填充图像数据：这里我们简单地填充渐变颜色作为示例
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                // 使用任意方式计算像素值，这里使用了一个简单的线性渐变
                image[y * width + x] = static_cast<uint8_t>((x + y) % 256);
            }
        }

        // 使用 stb_image_write 保存为 PNG 文件
        stbi_write_png(filename, width, height, 1, image.data(), width); // 最后一个参数是每行字节数
    }
};