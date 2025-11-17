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
                        Vector2 pointIn2D = pointOnCircle * cell.node->scale + origin;
                        Vector3 pos = cell.node->plane->to3D(pointIn2D);

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
                // to build the mesh, this context alive on the whole building operation.
                // so it visits each cell and each points of cells.

                void operator()(int pIdx, Vector2 &pointOnCircle)
                {                    
                    Vector3 pos = cell->node->to3D(origin, pointOnCircle);
                    obj->position(pos);
                    obj->normal(nom3);
                    obj->colour(color);
                }

            };
            ManualObject *obj;
            int baseIndex;
            int layers = 1;
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
            }

            // each cell visit op.
            void operator()(Cell::Instance &cell, ColourValue color)
            {
                visitPoint.cell = &cell;
                visitPoint.color = color;
                visitPoint.origin = cell.getOrigin2D();

                for (int i = 0; i < layers; i++)
                {
                    visitPoint.layer = i + 1;
                    float sizeI = std::powf(2, i) * 6;
                    Cell::forEachPointOnCircle(sizeI, 0.0f, visitPoint);
                }
            }

            void end()
            {
                this->obj->end();
            }
        };
    };
};