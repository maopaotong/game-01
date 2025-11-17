#pragma once
#include <Ogre.h>
#include <OgreVector.h>
#include "fg/util/CellMark.h"
#include "fg/Plane.h"
#include "fg/util/CostMap.h"
#include <OgreManualObject.h>
#include <OgreColourValue.h>
using namespace Ogre;

namespace fog
{
    template <typename... Args>
    static void forEachPointOnCircle(float size, float offsetAngle, void (*visit)(Vector2, Args...), Args... args)
    {

        float angle = 360.0f / size;

        for (int i = 0; i < size; i++)
        {

            float angle_rad = (angle * i + offsetAngle) * Ogre::Math::PI / 180.0f;

            float dx = std::cos(angle_rad);
            float dy = std::sin(angle_rad);

            visit(Ogre::Vector2(dx, dy), args...);
        }
    };

    class Cell
    {

    public:
        struct Instance
        {
            CellKey cKey;
            Node2D *node;
            Instance(int x, int y, Node2D *node) : Instance(CellKey(x, y), node)
            {
            }
            Instance(CellKey cKey, Node2D *node) : cKey(cKey), node(node)
            {
            }

            Ogre::Vector2 getOrigin2D()
            {
                float rad = node->scale;

                float centerX = cKey.first * 2 * rad + (cKey.second % 2 == 0 ? 0 : rad);
                float centerY = cKey.second * rad * std::sqrt(3.0f);

                return Ogre::Vector2(centerX, centerY);
            }

            void set(ManualObject *obj, Vector2 pointInCell2D, Vector2 origin, Vector3 nom, ColourValue color)
            {
                float height = 0.0f;
                Vector2 pointIn2D = pointInCell2D * this->node->scale + origin + this->node->position;
                Vector3 positionIn3D = node->plane->to3D(pointIn2D, height);
                obj->position(positionIn3D);
                obj->normal(nom);
                obj->colour(color);
            }

            void buildMesh(ManualObject *obj, ColourValue color)
            {
                Vector2 origin = this->getOrigin2D();
                int LAYERS = 3;
                for (int i = 0; i < LAYERS; i++)
                {
                    int sizeI = std::powf(2, i) * 6;
                    Vector3 nom(0, 1, 0);
                    void (*visit)(Vector2, Cell::Instance *, ManualObject *, Vector2 &origin, Vector3 &, ColourValue &) =
                        [](Vector2 point, Cell::Instance *this_, ManualObject *obj, Vector2 &origin, Vector3 &nom, ColourValue &color)
                    {
                        this_->set(obj, point, origin, nom, color);
                    };

                    forEachPointOnCircle<Cell::Instance *, ManualObject *, Vector2 &, Vector3 &, ColourValue &>(sizeI, 0.0f, visit, this, obj, origin, nom, color);
                }
            }
        };

        class Center
        {
            Node2D *root;
            CostMap *costMap;

        public:
            Center(Node2D *root, CostMap *costMap) : root(root), costMap(costMap)
            {
            }
            template <typename... Args>
            void forEachCell(void (*visit)(Cell::Instance &cell, Args... args), Args... args)
            {
                for (int x = 0; x < costMap->getWidth(); x++)
                {
                    for (int y = 0; y < costMap->getHeight(); y++)
                    {
                        Cell::Instance cell(x, y, root);
                        visit(cell, args...);
                    }
                }
            }
        };
    };

};