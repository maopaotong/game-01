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

    class Cell
    {
    public:
        /**
         *
         *    * * *     [0,3]     [1,3]    [2,3]
         *   * * *    [0,2]   [1,2]    [2,2]
         *    * * *     [0,1]     [1,1]    [2,1]
         *   * * *    [0,0]   [1,0]    [2,0]
         *
         */
        static void getNeibers(int x, int y, int *neibersX, int *neibersY)
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
        }

        //
        template <typename F, typename... Args>
        static void forEachPointOnCircle(float size, float offsetAngle, F &&visit, Args... args)
        {

            // rad=1, RAD=2
            float angle = 360.0f / size;

            for (int i = 0; i < size; i++)
            {

                float angle_rad = (angle * i + offsetAngle) * Ogre::Math::PI / 180.0f;

                float dx = std::cos(angle_rad);
                float dy = std::sin(angle_rad);

                visit(i, Ogre::Vector2(dx, dy), args...);
            }
        };

        static Ogre::Vector2 getOrigin2D(int x, int y, float rad)
        {

            float centerX = x * 2 * rad + (y % 2 == 0 ? 0 : rad);
            float centerY = y * rad * std::sqrt(3.0f);

            return Ogre::Vector2(centerX, centerY);
        }

        static CellKey getCellKey(Ogre::Vector2 origin2D, float rad)
        {

            // 从 y 解出整数 y
            int y = static_cast<int>(std::round(origin2D.y / (rad * std::sqrt(3.0f))));

            // 根据 y 的奇偶性，还原 x
            float xOffset = (y % 2 == 0) ? 0.0f : rad;
            int x = static_cast<int>(std::round((origin2D.x - xOffset) / (2.0f * rad)));
            return CellKey(x, y);
        }

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

            // center of the cell.
            Vector2 getOrigin2D()
            {
                return Cell::getOrigin2D(cKey.first, cKey.second, node->scale);
            }

            Vector3 getOrigin3D()
            {
                Vector2 origin2D = getOrigin2D();
                return node->to3D(origin2D);
            }
        };

        class Center
        {
            Node2D *root;
            CostMap *costMap;

        public:
            Center(Node2D *root) : root(root)
            {
                costMap = Context<CostMap>::get();
            }

            void translateToCenter()
            {
                Vector2 pos2D = this->getCenterIn2D();
                Vector3 pos3D = root->to3D(pos2D);
                root->plane->setOrigin(Vector3(-pos3D.x, 0.0f, -pos3D.z));
            }

            Cell::Instance getCell(CellKey cKey)
            {
                return Cell::Instance(cKey, root);
            }

            Cell::Instance getCell(int x, int y)
            {
                return Cell::Instance(x, y, root);
            }

            Cell::Instance getAnyCell()
            {
                return Cell::Instance(0, 0, root);
            }
            Node2D *getRoot2D()
            {
                return root;
            }
            std::vector<Vector2> getCellPointListByNomPoints(std::vector<Vector2> pointIn2DNom)
            {
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

            Vector2 getCenterIn2D()
            {
                float cX = this->costMap->getWidth();
                float cY = this->costMap->getHeight();
                return Vector2(cX, cY) * root->scale;
            }

            Cell::Instance getCellByNom(Vector2 nom)
            {
                return Cell::Instance(CellKey(static_cast<int>(nom.x), static_cast<int>(nom.y)), root);
            }

            bool findCellByWorldPosition(Vector3 positionIn3D, Cell::Instance &cell)
            {
                return forCellByPoint(positionIn3D, [&cell](Cell::Instance &cell2)
                                      { cell = cell2; });
            }
            
            CellKey getCellKeyByPoint(Vector2 pointIn2D){
                return Cell::getCellKey(pointIn2D, root->scale);
            }

            template <typename F>
            bool forCellByPoint(Vector3 positionIn3D, F &&visit)
            {

                // Vector2 pointIn2D = pointInCell2D * this->node->scale + origin + this->node->position;
                // Vector3 positionIn3D = node->plane->to3D(pointIn2D);
                Vector2 pointIn2D = root->to2D(positionIn3D);

                CellKey cKey = Cell::getCellKey(pointIn2D, root->scale);
                int x = cKey.first;
                int y = cKey.second;
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

            int getWidth()
            {
                return this->costMap->getWidth();
            }
            int getHeight()
            {
                return this->costMap->getHeight();
            }
        };
    };

};