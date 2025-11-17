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
            void operator()(ManualObject *obj, Cell::Instance &cell, ColourValue color)
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
                        Vector3 positionIn3D = cell.node->plane->to3D(pointIn2D);

                        obj->position(positionIn3D);
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
        };
    };
};