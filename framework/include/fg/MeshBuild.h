#pragma once
#include <Ogre.h>
#include <OgreVector.h>
#include "fg/util/CellMark.h"
#include "fg/Plane.h"
#include "fg/util/CostMap.h"
#include <OgreManualObject.h>
#include <OgreColourValue.h>
#include "fg/Terrains.h"
#include "Cell.h"
using namespace Ogre;

namespace fog
{

    class MeshBuild
    {

    public:
        class PointOnCircle
        {
        public:
            ManualObject *obj;
            int baseIndex;
            PointOnCircle(ManualObject *obj) : obj(obj) {}
            void begin(std::string material)
            {
                obj->clear();
                obj->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST);
                baseIndex = obj->getCurrentVertexCount();
            }

            void operator()(Cell::Instance &cell, ColourValue color)
            {
                Vector2 origin = cell.getOrigin2D();
                Vector3 nom3(0, 1, 0);

                struct Visit
                {
                    ManualObject *obj;
                    ColourValue color;
                    Cell::Instance &cell;
                    Vector2 origin;
                    Vector3 nom3;
                    int layer;
                    void operator()(int pIdx, Vector2 &pointOnCircle)
                    {
                        Vector3 pos = cell.node->to3D(origin, pointOnCircle);

                        obj->position(pos);
                        obj->normal(nom3);
                        obj->colour(color);

                        obj->position(pos.x, pos.y, pos.z + 10.0f); // density
                        obj->normal(nom3);
                        obj->colour(color);

                        obj->position(pos.x + 10.0f, pos.y, pos.z);
                        obj->normal(nom3);
                        obj->colour(color);
                    }
                } visit{
                    obj,
                    color,
                    cell,
                    origin,
                    nom3,
                };

                int LAYERS = 1;
                for (int i = 0; i < LAYERS; i++)
                {
                    visit.layer = i + 1;
                    float sizeI = std::powf(2, i) * 6;
                    Cell::forEachPointOnCircle(sizeI, 0.0f, visit);
                }
            }

            void end()
            {
                this->obj->end();
            }
        };

        class SpiderNet
        {
        public:
            struct PointVisit
            {
                ManualObject *obj;
                ColourValue color;
                Cell::Instance *cell;
                Vector2 origin;
                Vector3 nom3;
                int layer;
                int layerSize;
                int preLayerSize;
                int totalLayer = 2;//settings global
                int topLayer = 2;
                int botLayer = 0;
                // to build the mesh, this context alive on the whole building operation.
                // so it visits each cell and each points of cells.
                int idx; // point index

                void operator()(int pIdx, Vector2 &pointOnCircle)
                {
                    Vector2 pointOnLayer = pointOnCircle * ((float)layer / (float)totalLayer);
                    Vector3 pos = cell->node->to3D(origin, pointOnLayer);
                    obj->position(pos);
                    obj->normal(nom3);
                    obj->colour(color);

                    //
                    int size1 = preLayerSize;
                    int size2 = layerSize;
                    int i = layer;
                    int j = pIdx; //
                    // skip i==0

                    if (i > botLayer) //
                    {
                        // (j-1-size1)        .       .     .   .   .   .
                        //         |   \     | \      | \   | \ | \ | \ |
                        //      (j-1)__(j)   .___*    __    .   .   .
                        //        0     1    2   3    4  5

                        if (j % 2 == 1) // 1,3,5
                        {
                            obj->triangle(idx, idx - 1 - size1 - j / 2, idx - 1);
                        }
                        //
                        if (j % 2 == 0 && j > 1) // 2,4,6
                        {
                            obj->triangle(idx, idx - size1 - j / 2, idx - 1);
                            obj->triangle(idx - 1, idx - size1 - j / 2, idx - size1 - j / 2 - 1);
                        }
                        if (j == size2 - 1) // 0=idx-j
                        {                   // last one, then find the first one.
                            int j0 = idx - j;
                            obj->triangle(idx - j, (idx - j) - size1, idx);
                            obj->triangle(idx, (idx - j) - size1, idx - j - 1);
                        }
                    }
                    idx++;
                    //
                }
            }; // end of Point Visit

            ManualObject *obj;
            int baseIndex;
            PointVisit visitPoint;

            SpiderNet(ManualObject *obj) : obj(obj) {}
            void begin(std::string material)
            {
                obj->clear();
                obj->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST);
                baseIndex = obj->getCurrentVertexCount();
                //
                visitPoint.obj = obj;
                visitPoint.nom3 = Vector3(0, 1, 0);
                visitPoint.idx = baseIndex;
            }

            // each cell visit op.
            void operator()(Cell::Instance &cell, ColourValue color)
            {
                visitPoint.cell = &cell;
                visitPoint.color = color;
                visitPoint.origin = cell.getOrigin2D();
                visitPoint.layerSize = 0;
                //
                for (int i = visitPoint.botLayer; i < visitPoint.topLayer + 1; i++)
                {
                    visitPoint.layer = i;
                    visitPoint.preLayerSize = visitPoint.layerSize;
                    visitPoint.layerSize = layerSize(i);

                    Cell::forEachPointOnCircle(visitPoint.layerSize, 0.0f, visitPoint);
                }
            }

            int layerSize(int layer)
            {
                return layer = std::powf(2, layer) * 6;
            }

            void end()
            {
                this->obj->end();
            }
        }; // end of spider net.
    };
};