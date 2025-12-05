
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
                float rectWidth = static_cast<float>(tWidth) * 2.0f / static_cast<float>(width);
                float rectHeight = static_cast<float>(tHeight) * 2.0f / static_cast<float>(height) * std::sqrt(3) / 2.0f;

                for (int x = 0; x < width; x++)
                {
                    for (int y = 0; y < height; y++)
                    {

                        float centreX = static_cast<float>(x) * rectWidth;
                        float centreY = static_cast<float>(y) * rectHeight;

                        Vector2 centreP = Vector2(centreX, centreY) * 1.0f;
                        CellKey cKey = Cell::getCellKey(centreP, 1.0);

                        int tx = std::clamp<int>(cKey.first, 0, tWidth - 1);
                        int ty = std::clamp<int>(cKey.second, 0, tHeight - 1);

                        cKey.first = tx;
                        cKey.second = ty;

                        Tiles::Tile &tl = tiles[tx][ty];

                        // tile centre position.
                        Vector2 tCP = Cell::getOrigin2D(cKey.first, cKey.second, 1.0f);

                        //
                        if (Rect::isPointInSide(tCP, centreP, rectWidth, rectHeight))
                        {                             // is the center rect of the tile.
                            hMap[x][y].height = tileHeight(tl);
                        }
                        else
                        {
                            hMap[x][y].height = 0.49f;
                        }
                        hMap[x][y].cKey = cKey;
                        hMap[x][y].originInTile = centreP - tCP;
                    }
                }
            }

            float tileHeight(Tiles::Tile &tl)
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