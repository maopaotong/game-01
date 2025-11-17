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
                int topLayer = 5;
                // to build the mesh, this context alive on the whole building operation.
                // so it visits each cell and each points of cells.
                int idx; // point index

                void operator()(int pIdx, Vector2 &pointOnCircle)
                {
                    Vector2 pointOnLayer = pointOnCircle * (layer / topLayer);
                    Vector3 pos = cell->node->to3D(origin, pointOnLayer);
                    obj->position(pos);
                    obj->normal(nom3);
                    obj->colour(color);

                    //
                    int size1 = preLayerSize;
                    int size2 = layerSize;
                    int i = layer;
                    int j = pIdx; //

                    if (i > 0) // skip first layer, process from second ... layer
                    {
                        // (j-1-size1)    .   .   .   .   .   .
                        //         |   \   | \ | \ | \ | \ | \ |
                        //      (j-1)__(j)___*__*   .   .   .

                        if (j > 0 && j < size1 - 1)
                        {
                            obj->triangle(idx, idx - 1 - size1, idx - 1);
                        }

                        //         . __* __ .__. __. __ .__ .
                        //           \ | \ | \ | \ | \ | \ |
                        //         .   .   .   .   .   .   .
                        if (j > 0 && j < size2)
                        {
                            obj->triangle(idx, idx - size1, idx - 1);
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

                //
                for (int i = 0; i < visitPoint.topLayer + 1; i++)
                {
                    visitPoint.layer = i;
                    visitPoint.preLayerSize = visitPoint.layerSize;
                    visitPoint.layerSize = layerSize(i);
                    if (i == 0)
                    {
                        visitPoint(0, Vector2(0, 0));
                        continue;
                    }

                    Cell::forEachPointOnCircle(visitPoint.layerSize, 0.0f, visitPoint);
                }
            }

            int layerSize(int layer)
            {
                return layer == 0 ? 1 : std::powf(2, layer) * 6;
            }

            void end()
            {
                this->obj->end();
            }
        }; // end of spider net.
    };
};