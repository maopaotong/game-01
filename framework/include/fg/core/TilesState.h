
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
#include "fg/core/Tiles.h"
namespace fog
{
    using namespace Ogre;

    //
    class TilesState : public ManualState
    {

    public:
        static constexpr float HEIGHT_SCALE = 100.0f;

    public:
        TilesState() : ManualState()
        {
            this->material = "Tiles";
        }
        void init() override
        {
            ManualState::init();
        }
        void rebuildMesh() override
        {

            Tiles::Terrains *terrains = Context<Tiles::Terrains>::get();

            int qWidth = terrains->width;
            int qHeight = terrains->height;

            std::vector<std::vector<Tiles::Vertex>> &vertexs = terrains->hMap;
            obj->clear();
            obj->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST);

            int baseIdx = obj->getCurrentVertexCount();

            std::vector<std::vector<Vector3>> norms(qWidth, std::vector<Vector3>(qHeight, Vector3::UNIT_Y));
            std::vector<std::vector<Vector3>> positions(qWidth, std::vector<Vector3>(qHeight, Vector3::ZERO));

            Cell::Center *cc = Context<Cell::Center>::get();

            // collect position.
            for (int qy = 0; qy < qHeight; qy++)
            {
                for (int qx = 0; qx < qWidth; qx++)
                {

                    CellKey cKey = vertexs[qx][qy].cKey;

                    Cell::Instance cis = cc->getCell(cKey);

                    Vector2 tP = cis.getOrigin2D();
                    float scale = cis.node->getScale();
                    Vector2 qP = tP + vertexs[qx][qy].originInTile * scale;
                    // scale
                    float h = vertexs[qx][qy].height * HEIGHT_SCALE;
                    Vector3 position = cis.node->to3D(qP);
                    position.y = h;
                    positions[qx][qy] = position;
                }
            }
            // calculate norms

            for (int qy = 0; qy < qHeight; qy++)
            {
                for (int qx = 0; qx < qWidth; qx++)
                {

                    Vector3 p1 = positions[qx][qy];
                    obj->position(p1);
                    Vector3 normNs = calculateNorm(positions, qx, qy, qWidth, qHeight);
                    obj->normal(normNs);
                    obj->textureCoord(p1.x, -p1.z);

                } // end of for
            } // end of for

            // triangle
            for (int qy = 0; qy < qHeight - 1; qy++)
            {
                for (int qx = 0; qx < qWidth - 1; qx++)
                {

                    int a = qy * qWidth + qx;
                    int b = qy * qWidth + (qx + 1);
                    int c = (qy + 1) * qWidth + (qx + 1);
                    int d = (qy + 1) * qWidth + qx;

                    int ba = baseIdx + a;
                    int bb = baseIdx + b;
                    int bc = baseIdx + c;
                    int bd = baseIdx + d;

                    //

                    obj->triangle(ba, bb, bc);

                    obj->triangle(ba, bc, bd);
                }
            }

            obj->end();
            //
        }

        Vector3 calculateNorm(std::vector<std::vector<Vector3>> &positions, int qx, int qy, int qWidth, int qHeight)
        {
            const int N = 4;
            // South,East,North,West
            int neibersX[N] = {qx, qx + 1, qx, qx - 1};
            int neibersY[N] = {qy - 1, qy, qy + 1, qy};

            // Cell::getNeibers(qx, qy, neibersX, neibersY); // TODO
            //

            Vector3 neibersP[N];
            int neibersCount = 0;
            for (int i = 0; i < N; i++)
            {
                int nX = neibersX[i];
                int nY = neibersY[i];

                if (nX >= 0 && nX < qWidth && nY >= 0 && nY < qHeight) // TODO
                {
                    neibersP[neibersCount] = positions[nX][nY];
                    neibersCount++;
                }
            }

            Vector3 normNs; //
            Vector3 p1 = positions[qx][qy];
            if (DEBUG_COUT)
            {

                std::cout << fmt::format("======[{},{}]==================", qx, qy) << std::endl;
                std::cout << fmt::format("p1:({:>8.1f},{:>8.1f},{:>8.1f})", p1.x, p1.y, p1.z) << std::endl;
                std::cout << fmt::format("neibersCount:{}", neibersCount) << std::endl;
            }
            if (neibersCount == N)
            {
                normNs = Vector3(0, 0, 0);
                for (int i = 0; i < neibersCount; i++)
                {
                    Vector3 p2 = neibersP[i];
                    Vector3 p3 = neibersP[(i + 1) % neibersCount];
                    Vector4 plane = Math::calculateFaceNormalWithoutNormalize(p1, p2, p3);
                    Vector3 normN(plane.x, plane.y, plane.z);
                    normN.normalise();
                    normNs += normN;
                    if (DEBUG_COUT)
                    {
                        std::cout << fmt::format("neibers   [{}]-----------------------------------------", i) << std::endl;
                        std::cout << fmt::format("neibers   [{:>3}]p1:({:>8.1f},{:>8.1f},{:>8.1f})", i, p1.x, p1.y, p1.z) << std::endl;
                        std::cout << fmt::format("neibers   [{:>3}]p2:({:>8.1f},{:>8.1f},{:>8.1f})", i, p2.x, p2.y, p2.z) << std::endl;
                        std::cout << fmt::format("neibers   [{:>3}]p3:({:>8.1f},{:>8.1f},{:>8.1f})", i, p3.x, p3.y, p3.z) << std::endl;
                        std::cout << fmt::format("normN     [{:>3}]  :({:>8.1f},{:>8.1f},{:>8.1f})", i, normN.x, normN.y, normN.z) << std::endl;
                        std::cout << fmt::format("normNsRaw [{:>3}]  :({:>8.1f},{:>8.1f},{:>8.1f})", i, normNs.x, normNs.y, normNs.z) << std::endl;
                    }
                }
                normNs.normalise();
                if (DEBUG_COUT)
                {
                    std::cout << fmt::format("normNsNormalised [{},{}]({:>8.1f},{:>8.1f},{:>8.1f})", qx, qy, normNs.x, normNs.y, normNs.z) << std::endl;
                }
            }
            else
            {
                normNs = Vector3(0, 1, 0);
            }
            return normNs;
        } // end method.
    }; // end class
}; // end of namespace