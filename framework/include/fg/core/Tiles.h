
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
namespace fog
{

    class Tiles
    {
    public:
        enum class Type
        {
            OCEAN,
            SHORE,
            PLAIN,
            HILL,
            MOUNTAIN,
            FROZEN,
            DESERT,
            LAKE,
            RIVER,

            UNKNOW
        };
        //
        struct Tile
        {
            Type type;

            Tile() : type(Type::UNKNOW)
            {
            }
        };

        struct Vertex
        {
            float height;
            CellKey cKey;
            Vector2 originInTile;

            Vertex() : height(0.0f), cKey(-1, -1)
            {
            }
            Vertex(int tx, int ty, float th) : height(0.0f), cKey(tx, ty)
            {
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

                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {

                        float pX = 2.0 * static_cast<float>(x) * static_cast<float>(tWidth) / static_cast<float>(width);
                        float pY = 2.0 *  static_cast<float>(y) * static_cast<float>(tHeight) / static_cast<float>(height);
                        Vector2 pPos = Vector2(pX, pY) * 1.0f;
                        CellKey cKey = Cell::getCellKey(pPos, 1.0);

                        int tx = cKey.first;
                        int ty = cKey.second;
                        if (tx < 0)
                        {
                            tx = 0;
                        }
                        if (ty < 0)
                        {
                            ty = 0;
                        }
                        if (tx > tWidth - 1)
                        {
                            tx = tWidth - 1;
                        }
                        if (ty > tHeight - 1)
                        {
                            ty = tHeight - 1;
                        }

                        cKey.first = tx;
                        cKey.second = ty;
                        Tiles::Tile &tl = tiles[tx][ty];

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
                        Vector2 tP = Cell::getOrigin2D(cKey.first, cKey.second, 1.0f);

                        hMap[x][y].cKey = cKey;
                        hMap[x][y].height = tlHeight;
                        hMap[x][y].originInTile = Vector2(pX - tP.x, pY - tP.y);
                    }
                }
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