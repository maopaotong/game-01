
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
#include "fg/demo/DiamondSquare.h"

namespace fog
{
    using namespace Ogre;

    typedef Property::Ref<bool> boolRef;

    //
    class TilesState : public ManualState
    {
    public:
    public:
        TilesState() : ManualState()
        {
        }
        void init() override
        {
            ManualState::init();
        }
        void rebuildMesh() override
        {
            Cell::Center *cc = Context<Cell::Center>::get();

            int w = Context<Cell::Center>::get()->getWidth();
            int h = Context<Cell::Center>::get()->getHeight();
            int size = w;
            std::vector<std::vector<float>> heightmap(size, std::vector<float>(size, 0.0f));
            DiamondSquare::generateAndNormalise(heightmap, size, 0.6, 8151245);
            DiamondSquare::eraseDetailWithinTerrainTypes(heightmap, size);

            obj->clear();
            obj->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST);

            int baseIdx = obj->getCurrentVertexCount();

            std::vector<std::vector<Vector3>> positions(size, std::vector<Vector3>(size, Vector3::ZERO));
            std::vector<std::vector<Vector3>> norms(size, std::vector<Vector3>(size, Vector3::UNIT_Y));

            // collect position.
            for (int x = 0; x < w; x++)
            {
                for (int y = 0; y < h; y++)
                {
                    Cell::Instance a = cc->getCell(x, y);
                    Vector3 pA = a.getOrigin3D();
                    pA.y = heightmap[x][y] * 10;
                    positions[x][y] = pA;
                }
            }
            // calculate norms
            int neibersX[6] = {-1};
            int neibersY[6] = {-1};

            /**
             *
             *    * * *     [0,3]     [1,3]    [2,3]
             *   * * *    [0,2]   [1,2]    [2,2]
             *    * * *     [0,1]     [1,1]    [2,1]
             *   * * *    [0,0]   [1,0]    [2,0]
             *
             */
            for (int x = 0; x < w; x++)
            {
                for (int y = 0; y < h; y++)
                {
                    // e.g. :[1,2]
                    if (y % 2 == 0)
                    {

                        neibersX[0] = x; //[1,1]
                        neibersY[0] = y - 1;

                        neibersX[1] = x + 1; //[2,2]
                        neibersY[1] = y;

                        neibersX[2] = x; //[1,3]
                        neibersY[2] = y + 1;

                        neibersX[3] = x - 1; //[0,3]
                        neibersY[3] = y + 1;

                        neibersX[4] = x - 1; //[0,2]
                        neibersY[4] = y;

                        neibersX[5] = x - 1; //[0,1]
                        neibersY[5] = y - 1;
                    }
                    else
                    { // e.g. [1,1]

                        neibersX[0] = x + 1; //[2,0]
                        neibersY[0] = y - 1;

                        neibersX[1] = x + 1; //[2,1]
                        neibersY[1] = y;

                        neibersX[2] = x + 1; //[2,2]
                        neibersY[2] = y + 1;

                        neibersX[3] = x; //[1,2]
                        neibersY[3] = y + 1;

                        neibersX[4] = x - 1; //[0,1]
                        neibersY[4] = y;

                        neibersX[5] = x; //[1,0]
                        neibersY[5] = y - 1;
                    }
                    //
                    Vector3 neibersP[6];
                    int neibersCount = 0;
                    for (int i = 0; i < 6; i++)
                    {
                        int nX = neibersX[i];
                        int nY = neibersY[i];

                        if (nX >= 0 && nX < size && nY >= 0 && nY < size)
                        {
                            neibersP[neibersCount] = positions[nX][nY];
                            neibersCount++;
                        }
                    }

                    Vector3 normNs(0, 1, 0); //
                    Vector3 p1 = positions[x][y];
                    for (int i = 0; i < neibersCount; i++)
                    {
                        Vector3 p2 = neibersP[i];
                        Vector3 p3 = neibersP[(i + 1) % neibersCount];
                        Vector4 plane = Math::calculateFaceNormalWithoutNormalize(p1, p2, p3);
                        Vector3 normN(plane.x, plane.y, plane.z);
                        normN.normalise();
                        normNs += normN;
                        normNs.normalise();
                    }
                    obj->position(p1);
                    obj->normal(normNs);
                    if (p1.y < 5.0f)
                    {
                        obj->colour(ColourValue::Blue);
                    }
                    else if (p1.y < 7.5f)
                    {
                        obj->colour(ColourValue::Green);
                    }
                    else
                    {
                        obj->colour(ColourValue::White);
                    }
                }
            }

            // triangle
            for (int x = 0; x < w - 1; x++)
            {
                for (int y = 0; y < h - 1; y++)
                {

                    int a = baseIdx + x * w + y;
                    int b = baseIdx + (x + 1) * w + y;
                    int c = baseIdx + (x + 1) * w + (y + 1);
                    int d = baseIdx + x * w + (y + 1);

                    //
                    obj->triangle(a, b, c);
                    obj->triangle(a, c, d);
                }
            }

            obj->end();
        }
    };

}; // end of namespace