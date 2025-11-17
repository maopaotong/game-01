#pragma once
#include <Ogre.h>
#include <OgreVector.h>
#include "fg/util/CellMark.h"
#include "fg/Plane.h"
#include "fg/util/CostMap.h"
#include <OgreManualObject.h>
#include <OgreColourValue.h>
#include "fg/Terrains.h"
using namespace Ogre;

namespace fog
{
    template <typename F>
    static void forEachPointOnCircle(float size, float offsetAngle, F &&visit)
    {

        float angle = 360.0f / size;

        for (int i = 0; i < size; i++)
        {

            float angle_rad = (angle * i + offsetAngle) * Ogre::Math::PI / 180.0f;

            float dx = std::cos(angle_rad);
            float dy = std::sin(angle_rad);

            visit(Ogre::Vector2(dx, dy));
        }
    };

    class Cell
    {

    public:
        struct Instance : public Object2D
        {
            CellKey cKey;
            Instance()
            {
            }
            Instance(int x, int y, Node2D *node) : Instance(CellKey(x, y), node)
            {
            }
            Instance(CellKey cKey, Node2D *node) : cKey(cKey), Object2D(node)
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

                Vector2 pointIn2D = pointInCell2D * this->node->scale + origin + this->node->position;
                Vector3 positionIn3D = node->plane->to3D(pointIn2D);
                obj->position(positionIn3D);
                obj->normal(nom);
                obj->colour(color);
            }

            void buildMesh(ManualObject *obj, ColourValue color)
            {
                Vector2 origin = this->getOrigin2D();
                Vector3 nom(0, 1, 0);

                struct Visit
                {
                    ManualObject *obj;
                    ColourValue color;
                    Cell::Instance *this_;
                    Vector2 origin;
                    Vector3 nom;

                    void operator()(Vector2 &point)
                    {
                        this_->set(obj, point, origin, nom, color);
                    }
                } visit{
                    obj,
                    color,
                    this,
                    origin,
                    nom,
                };

                int LAYERS = 3;
                for (int i = 0; i < LAYERS; i++)
                {
                    float sizeI = std::powf(2, i) * 6;

                    forEachPointOnCircle(sizeI, 0.0f, visit);
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
            Node2D *getRoot2D(){
                return root;
            }
            std::vector<Vector2> getCellPointListByNomPoints(std::vector<Vector2> pointIn2DNom){
                std::vector<Vector2> ret;
                ret.reserve(pointIn2DNom.size());
                for (auto pNom : pointIn2DNom)
                {
                    Cell::Instance cell = getCellByNom(pNom);

                    ret.push_back(cell.getOrigin2D());
                }
                return ret;
            }

            std::vector<Cell::Instance> getInstanceListByNomPoints(std::vector<Vector2> pointIn2DNom)
            {
                std::vector<Cell::Instance> ret;
                ret.reserve(pointIn2DNom.size());
                for (auto pNom : pointIn2DNom)
                {
                    Cell::Instance cell = getCellByNom(pNom);
                    ret.push_back(cell);
                }
                return ret;
            }

            Cell::Instance getCellByNom(Vector2 nom)
            {
                return Cell::Instance(CellKey(static_cast<int>(nom.x), static_cast<int>(nom.y)), root);
            }

            bool findCellByPoint(Vector3 positionIn3D, Cell::Instance &cell)
            {
                return forCellByPoint(positionIn3D, [&cell](Cell::Instance &cell2)
                                      { cell = cell2; });
            }
            template <typename F>
            bool forCellByPoint(Vector3 positionIn3D, F &&visit)
            {

                // Vector2 pointIn2D = pointInCell2D * this->node->scale + origin + this->node->position;
                // Vector3 positionIn3D = node->plane->to3D(pointIn2D);
                Vector2 pointIn2D = root->plane->to2D(positionIn3D);

                Vector2 pointIn2DNom = (pointIn2D - root->position) / root->scale;

                int x = pointIn2DNom.x;
                int y = pointIn2DNom.y;

                if (x < 0 || x >= costMap->getWidth() || y < 0 || y >= costMap->getHeight())
                {
                    return false;
                }

                visit(Cell::Instance(x, y, root));
                return true;
            }

            template <typename F>
            void forEachCell(F &&visit)
            {
                for (int x = 0; x < costMap->getWidth(); x++)
                {
                    for (int y = 0; y < costMap->getHeight(); y++)
                    {
                        Cell::Instance cell(x, y, root);
                        visit(cell);
                    }
                }
            }
        };
    };

};