
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

    using TileType = uint8;
    class Tiles
    {
    public:
        class Type
        {
        public:
            static constexpr TileType OCEAN = 1;
            static constexpr TileType SHORE = 2;
            static constexpr TileType PLAIN = 3;
            static constexpr TileType HILL = 4;
            static constexpr TileType MOUNTAIN = 5;
            static constexpr TileType FROZEN = 6;
            static constexpr TileType DESERT = 7;
            static constexpr TileType LAKE = 8;
            static constexpr TileType RIVER = 9;
            static constexpr TileType UNKNOW = 0;
        };

    public:
        //
        struct Tile
        {
            TileType type;

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
            TileType type;

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
            float computeCorrectHeight(Ogre::Vector2 center, float rectWidth, float rectHeight,
                                       std::function<float(CellKey)> getHeight)
            {
                const int SAMPLES = 3; // 3x3 = 9 points；可调为 5（25点）更准
                float totalHeight = 0.0f;

                for (int i = 0; i < SAMPLES; ++i)
                {
                    for (int j = 0; j < SAMPLES; ++j)
                    {
                        // 在矩形内均匀采样
                        float u = (i + 0.5f) / SAMPLES; // 避免采样边界
                        float v = (j + 0.5f) / SAMPLES;
                        Ogre::Vector2 p(
                            center.x + (u - 0.5f) * rectWidth,
                            center.y + (v - 0.5f) * rectHeight);

                        CellKey k = Cell::getCellKey(p, 1); // 使用你已修正的精确版本

                        totalHeight += getHeight(k);
                    }
                }

                return totalHeight / (SAMPLES * SAMPLES);
            }
            void init(std::vector<std::vector<Tile>> &tiles, int tWidth, int tHeight)
            {
                float rectWidth = static_cast<float>(tWidth) * 2.0f / static_cast<float>(width);
                float rectHeight = static_cast<float>(tHeight) * 2.0f / static_cast<float>(height) * std::sqrt(3) / 2.0f;
                float rectRad = (rectHeight + rectWidth) / 2.0;
                std::vector<std::vector<Vertex *>> tileCentreMap(tWidth, std::vector<Vertex *>(tHeight, nullptr));
                // resove the terrain height of the centre rect for each tile.
                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {
                        float centreX = static_cast<float>(x) * rectWidth;
                        float centreY = static_cast<float>(y) * rectHeight;
                        Vector2 points[5];
                        points[0] = Vector2(centreX, centreY);
                        points[1] = Vector2(centreX, centreY - 1);
                        points[2] = Vector2(centreX + 1, centreY);
                        points[3] = Vector2(centreX, centreY + 1);
                        points[4] = Vector2(centreX - 1, centreY);

                        CellKey cKeys[5];
                        for (int i = 0; i < 5; i++)
                        {

                            cKeys[i] = Cell::getCellKey(points[i], 1.0); // centre
                            int tx = std::clamp<int>(cKeys[i].first, 0, tWidth - 1);
                            int ty = std::clamp<int>(cKeys[i].second, 0, tHeight - 1);
                            cKeys[i].first = tx;
                            cKeys[i].second = ty;
                        }

                        Tiles::Tile &tl0 = tiles[cKeys[0].first][cKeys[0].second];
                        // tile centre position.
                        Vector2 tillCentreP = Cell::getOrigin2D(cKeys[0].first, cKeys[0].second, 1.0f);
                        //
                        hMap[x][y].cKey = cKeys[0];
                        hMap[x][y].originInTile = points[0] - tillCentreP;
                        hMap[x][y].type = tl0.type;

                        // tile's centre is in this rect.
                        if (Rect::isPointInSide(tillCentreP, points[0], rectWidth, rectHeight)) //
                        {                                                                       // is the center rect of the tile.
                            // remember the centre rect for each tile.
                            tileCentreMap[cKeys[0].first][cKeys[0].second] = &hMap[x][y];
                            hMap[x][y].height = defineTileHeight(tl0);
                        }
                        else // not the centre rect, check all the corner's tile type.
                        {
                            Tiles::Tile &tl1 = tiles[cKeys[1].first][cKeys[1].second];
                            Tiles::Tile &tl2 = tiles[cKeys[2].first][cKeys[2].second];
                            Tiles::Tile &tl3 = tiles[cKeys[3].first][cKeys[3].second];
                            Tiles::Tile &tl4 = tiles[cKeys[4].first][cKeys[4].second];

                            // check if 4 corner's type is same;
                            // check if this rect is inside a certain type.
                            if (tl1.type == tl2.type && tl1.type == tl3.type && tl1.type == tl4.type)
                            {
                                hMap[x][y].height = defineTileHeight(tl0);
                            }
                            else
                            { // calculate
                                float h = computeCorrectHeight(points[0], rectWidth, rectHeight, [this, &tiles, tWidth, tHeight](CellKey cKey)
                                                               {
                                                                   int tx = std::clamp<int>(cKey.first, 0, tWidth - 1);
                                                                   int ty = std::clamp<int>(cKey.second, 0, tHeight - 1);
                                                                   
                                                                   Tiles::Tile &ttl = tiles[tx][ty];
                                                                   return defineTileHeight(ttl); //
                                                               });
                                hMap[x][y].height = h;
                            }
                            //
                        }

                        //
                    }
                }
                // calculate the height of non-centre but the inner circle is inside the same cell.
                /*
                 */
                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {
                        if (!hMap[x][y].isHeightResolved())
                        {

                            float distanceToTileCentre = hMap[x][y].originInTile.length();
                            // rad of the tile is 1.0;
                            if (distanceToTileCentre + rectRad < 1.0)
                            {
                                int tx = hMap[x][y].cKey.first;
                                int ty = hMap[x][y].cKey.second;
                                Vertex *centreRect = tileCentreMap[tx][ty];
                                assert(centreRect && "centreRest is missing?");

                                hMap[x][y].height = centreRect->height; // use the same height as the centre rect.
                            }
                        }
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
                                    float weight = 1.0f / (distance + 1e-6);
                                    weight = 1.0f;
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

            // World texture is used as the meta data for the shader to determine the child texture.
            void createWorldTexture(std::string name)
            {

                // 上传到 GPU 纹理
                PixelFormat format = Ogre::PF_BYTE_RGBA;

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
                    Ogre::TU_DEFAULT // only this option works fine; other option does not work(cannot read texture from fragment shader);
                    // Ogre::TU_DYNAMIC_WRITE_ONLY
                    // Ogre::TU_STATIC_WRITE_ONLY
                );

                unsigned char *data = new unsigned char[width * height * 4];
                int typePlot[11] = {0};
                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {
                        Vertex &v = hMap[x][y];
                        if (v.type < 10)
                        {
                            typePlot[v.type]++;
                        }
                        else
                        {
                            typePlot[10]++;
                        }

                        int idx = (y * width + x) * 4;

                        data[idx] = v.type;  // R
                        data[idx + 1] = 255; // G
                        data[idx + 2] = 255; // B
                        data[idx + 3] = 255; // A
                    }
                }
                for (int i = 0; i < 11; i++)
                {
                    std::cout << fmt::format("typePlot[{}] is {:>3}", i, typePlot[i]) << std::endl;
                }

                Ogre::HardwarePixelBufferSharedPtr buffer = tex->getBuffer();
                Ogre::PixelBox pBox(width, height, 1, format, data);
                buffer->lock(
                    Ogre::HardwareBuffer::HBL_READ_ONLY
                    // Ogre::HardwareBuffer::HBL_DISCARD memory access error when set to any other option.
                );

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
                    tlHeight = HEIGHT_OCEAN;
                    break;
                case (Type::SHORE):
                    tlHeight = HEIGHT_SHORE;
                    break;
                case (Type::PLAIN):
                    tlHeight = HEIGHT_PLAIN;
                    break;
                case (Type::HILL):
                    tlHeight = HEIGHT_HILL;
                    break;
                case (Type::MOUNTAIN):
                    tlHeight = HEIGHT_MOUNTAIN;
                    break;
                case (Type::FROZEN):
                    tlHeight = HEIGHT_FROZEN;
                    break;
                default:
                    tlHeight = HEIGHT_FROZEN;
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
                                              TileType type = Type::UNKNOW;
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