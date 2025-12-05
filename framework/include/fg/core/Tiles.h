
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/util/CostMap.h"
#include "fg/Cell.h"
#include "fg/util/CostMap.h"
#include "fg/MeshBuild.h"
#include "fg/VarBag.h"
#include "fg/Property.h"
#include "fg/core/ManualState.h"
#include "fg/util/DiamondSquare.h"
#include "fg/defines.h"
#include "fg/util/Iteration.h"
#include "fg/util/Rect.h"
namespace fog
{

    class Tiles
    {
    public:
        enum class Type : unsigned char
        {
            OCEAN = 1,
            SHORE = 2,
            PLAIN = 3,
            HILL = 4,
            MOUNTAIN = 5,
            FROZEN = 6,
            DESERT = 7,
            LAKE = 8,
            RIVER = 9,
            UNKNOW = 0

        };

    public:
        static int getIntValue(Type type)
        {
            return static_cast<int>(type);
        }
        //
        struct Tile
        {
            Type type;

            Tile() : type(Type::UNKNOW)
            {
            }
        };
        static constexpr float UNRESOLVED_HEIGHT = -100;
        struct Vertex
        {
            float height;
            CellKey cKey;
            Vector2 originInTile;
            Type type;

            Vertex() : Vertex(UNRESOLVED_HEIGHT, -1, -1)
            {
            }
            Vertex(int tx, int ty, float th) : height(UNRESOLVED_HEIGHT), cKey(tx, ty), type(Type::UNKNOW)
            {
            }
            bool isHeightResolved()
            {
                return std::abs(this->height - UNRESOLVED_HEIGHT) > 0.01f; // height is not unknown.
            }

            float distance(CellKey cKey2)
            {
                return (Vector2(cKey.first, cKey.second) + originInTile).distance(Vector2(cKey2.first, cKey2.second));
            }
        };

        struct Terrains
        {

            int width;
            int height;
            std::vector<std::vector<Vertex>> hMap;
            Terrains(int width, int height) : width(width), height(height),
                                              hMap(width, std::vector<Vertex>(height, Vertex()))
            {
            }

            void init(std::vector<std::vector<Tile>> &tiles, int tWidth, int tHeight)
            {
                float rectWidth = static_cast<float>(tWidth) * 2.0f / static_cast<float>(width);
                float rectHeight = static_cast<float>(tHeight) * 2.0f / static_cast<float>(height) * std::sqrt(3) / 2.0f;
                std::vector<std::vector<Vertex *>> tileCentreMap(tWidth, std::vector<Vertex *>(tHeight, nullptr));
                // resove the terrain height of the centre rect for each tile.
                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {

                        float centreX = static_cast<float>(x) * rectWidth;
                        float centreY = static_cast<float>(y) * rectHeight;

                        Vector2 rectCentreP = Vector2(centreX, centreY) * 1.0f;
                        CellKey cKey = Cell::getCellKey(rectCentreP, 1.0);

                        int tx = std::clamp<int>(cKey.first, 0, tWidth - 1);
                        int ty = std::clamp<int>(cKey.second, 0, tHeight - 1);

                        cKey.first = tx;
                        cKey.second = ty;

                        Tiles::Tile &tl = tiles[tx][ty];

                        // tile centre position.
                        Vector2 tillCentreP = Cell::getOrigin2D(cKey.first, cKey.second, 1.0f);

                        //
                        if (Rect::isPointInSide(tillCentreP, rectCentreP, rectWidth, rectHeight))
                        { // is the center rect of the tile.
                            hMap[x][y].height = defineTileHeight(tl);
                            tileCentreMap[tx][ty] = &hMap[x][y];
                        }
                        hMap[x][y].cKey = cKey;
                        hMap[x][y].originInTile = rectCentreP - tillCentreP;
                    }
                }
                // calculate the height of non-centre by neiber's height.
                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {
                        Vertex &vertex = this->hMap[x][y];

                        if (!vertex.isHeightResolved())
                        { // is the centre rect
                            // find all neiber tiles , and include the current till as well.
                            // 6+1 = 7 as the max size;
                            CellKey cKey = hMap[x][y].cKey;
                            float sumHeight = 0.0f;
                            int validNeibers = 0;
                            float sumWeight = 0;
                            auto visit = [&x, &y, &vertex, &validNeibers, &sumHeight, &tileCentreMap, &sumWeight](int idx, int nTX, int nTY)
                            {
                                Vertex *nTileCentre = tileCentreMap[nTX][nTY];
                                if (nTileCentre)
                                { // if centre rect exist for this current neiber.
                                    float distance = vertex.distance(nTileCentre->cKey);
                                    float weight = 1.0f / (distance * distance + 1e-6);
                                    sumHeight += nTileCentre->height * weight;
                                    validNeibers++;
                                    sumWeight += weight;
                                    if (DEBUG_COUT)
                                    {
                                        std::cout << fmt::format("Vertex[{},{}],Neiber[{:>2}].cKey({},{}) .height:{}, .dis:{}, sumHeight:{},sumWeight:{}", x, y, idx, nTX, nTY, nTileCentre->height, distance, sumHeight, sumWeight) << std::endl;
                                    }
                                }
                            };
                            if (DEBUG_COUT)
                            {
                                std::cout << fmt::format("===================") << std::endl;
                            }

                            int neibers = Cell::forEachNeibers(cKey.first, cKey.second, tWidth, tHeight, visit); // visit neibers.
                            visit(-1, cKey.first, cKey.second);                                                  // visit this cell.
                            assert(validNeibers > 0 && "bug?");
                            float height = sumHeight / sumWeight;
                            if (DEBUG_COUT)
                            {
                                std::cout << fmt::format("::::::::::{}:::::::::", height) << std::endl;
                            }
                            hMap[x][y].height = height;
                        }
                    }
                }
            }

            void createTexture(std::string name)
            {

                // 上传到 GPU 纹理
                PixelFormat format = Ogre::PF_BYTE_BGRA;

                TexturePtr tex = TextureManager::getSingleton().getByName(name);
                if (tex)
                {
                    std::cout << "Bug? any way remove texture:" << name << std::endl;
                    TextureManager::getSingleton().remove(name);
                }
                std::cout << "Create texture:" << name << std::endl;
                tex = TextureManager::getSingleton().createManual(
                    name,
                    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                    Ogre::TEX_TYPE_2D,
                    width, height, 0,
                    format,
                    Ogre::TU_DYNAMIC_WRITE_ONLY);

                unsigned char *data = new unsigned char[width * height * 4];

                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {
                        Vertex &v = hMap[x][y];
                        int type = getIntValue(v.type);
                        int idx = (y * width + x) * 4;
                        data[idx] = type;
                        data[idx + 1] = 0;
                        data[idx + 2] = 0;
                        data[idx + 3] = 0;
                    }
                }

                Ogre::HardwarePixelBufferSharedPtr buffer = tex->getBuffer();
                Ogre::PixelBox pBox(width, height, 1, format, data);
                buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
                buffer->blitFromMemory(pBox);
                buffer->unlock();
                delete[] data;
            }

            float defineTileHeight(Tiles::Tile &tl)
            {

                float tlHeight = 0.0;

                switch (tl.type)
                {
                case (Type::OCEAN):
                    tlHeight = 0.49f;
                    break;
                case (Type::SHORE):
                    tlHeight = 0.50f;
                    break;
                case (Type::PLAIN):
                    tlHeight = 0.51f;
                    break;
                case (Type::HILL):
                    tlHeight = 0.52f;
                    break;
                case (Type::MOUNTAIN):
                case (Type::FROZEN):
                    tlHeight = 0.53f;
                    break;
                default:
                    tlHeight = 0.51f;
                    break;
                }
                return tlHeight;
            }
        };

        class Generator
        {

        public:
            static constexpr float HEIGHT_SCALE = 100.0f;

            static void generateTiles(std::vector<std::vector<Tile>> &tiles, int w, int h)
            {
                assert(w == h && "cannot generate tiles because w<>h.");

                std::vector<std::vector<float>> heightmap(w, std::vector<float>(w, 0.0f));
                DiamondSquare::generateAndNormalise(heightmap, w, 0.45, 8151245);

                Iteration::forEach<float>(heightmap, w, w, [&tiles](int x, int y, float h)
                                          {
                                              Type type = Type::UNKNOW;
                                              if (h < 0.35)
                                              {
                                                  type = Type::OCEAN;
                                              }
                                              else if (h < 0.5)
                                              {
                                                  type = Type::SHORE;
                                              }
                                              else if (h < 0.85)
                                              {
                                                  type = Type::PLAIN;
                                              }
                                              else if (h < 0.9)
                                              {
                                                  type = Type::HILL;
                                              }
                                              else if (h < 0.95)
                                              {
                                                  type = Type::MOUNTAIN;
                                              }
                                              else
                                              {
                                                  type = Type::FROZEN;
                                              }
                                              tiles[x][y].type = type; //
                                          });
            } // end of method.
        }; // end of class
    }; // end of class
}; // end of namespace