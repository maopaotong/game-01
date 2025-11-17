
#pragma once
#include <vector>
#include <Ogre.h>
#include "CostMap.h"
namespace fog
{
    class CellUtil
    {
    public:
        static Ogre::Vector2 calculateCenter(int x, int y, float rad = CostMap::hexSize)
        {
            float centerX = x * 2 * rad + (y % 2 == 0 ? 0 : rad);
            float centerY = y * rad * std::sqrt(3.0f);

            return Ogre::Vector2(centerX, centerY);
        }

        static Ogre::Vector2 calculateCenter(int x, int y, Vector2 &offset, float rad = CostMap::hexSize)
        {

            return calculateCenter(x, y, rad) + offset;
        }

        static Ogre::Vector2 calculateCenter(int x, int y, CostMap *costMap, float rad = CostMap::hexSize)
        {
            return calculateCenter(x, y, CellUtil::offset(costMap), rad);
        }

        static void translatePathToCellCenter(std::vector<Vector2> &pathByKey, std::vector<Vector2> &pathByPosition, Vector2 &offset)
        {
            for (int i = 0; i < pathByKey.size(); i++)
            {
                auto p = pathByKey[i];
                // auto center = CostMap::calculateCenterForXZ(static_cast<int>(p.x), static_cast<int>(p.y), CostMap::hexSize);
                auto center = CellUtil::calculateCenter(static_cast<int>(p.x), static_cast<int>(p.y), offset, CostMap::hexSize);

                pathByPosition[i] = Vector2(center.x, center.y);
            }
        }
        /*
        static bool findCellByPointDEL(CostMap *costMap, Vector3 point, CellKey &cKey)
        {
            return findCellByPoint(costMap, point.x, point.z, cKey.first, cKey.second);
        }
        */

        // static bool findCellByPoint(CostMap *costMap, Vector2 point, CellKey &cKey)
        // {
        //     return findCellByPoint(costMap, point.x, point.y, cKey.first, cKey.second);
        // }

        static Vector2 offset(CostMap *costMap)
        {
            int width = costMap->getWidth();
            int height = costMap->getHeight();
            return Vector2(-width * CostMap::hexSize / 2.0f, -height * CostMap::hexSize / 2.0f);
        }

        static bool findCellByPoint(CostMap *costMap, float px, float py, int &cx, int &cy)
        {
            int width = costMap->getWidth();
            int height = costMap->getHeight();

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    if (isPointInCell(px, py, x, y, offset(costMap)))
                    {
                        cx = x;
                        cy = y;
                        return true;
                    }
                }
            }
            return false;
        }

        static bool isPointInCell(float px, float py, int cx, int cy, CostMap *costMap)
        {
            return isPointInCell(px, py, cx, cy, offset(costMap));
        }

        static bool isPointInCell(float px, float py, int cx, int cy, Vector2 &offset)
        {
            // auto corners = CostMap::calculateVerticesForXZ(cx, cy, CostMap::hexSize);
            auto corners = CellUtil::calculateVertices(cx, cy, offset, CostMap::hexSize);

            // 叉积判断是否在所有边的左侧
            for (int i = 0; i < 6; ++i)
            {
                // 逆时针
                auto p1 = corners[i];
                auto p2 = corners[(i + 1) % 6];

                // 向量 edge = p2 - p1
                // 向量 point = (mx, my) - p1
                float cross = (px - p1.x) * (p2.y - p1.y) - (py - p1.y) * (p2.x - p1.x);

                if (cross > 0)
                {
                    return false;
                }
            }
            return true;
        }

        // Get hexagon vertices
        // anti-clockwise
        static std::vector<Ogre::Vector2> calculateVertices(Vector2 &offset, float rad, float scale = 1.0f)
        {
            return calculateVertices(0, 0, offset, rad, scale);
        }

        static std::vector<Ogre::Vector2> calculateVertices(int x, int y, Vector2 &offset, float rad, float scale = 1.0f)
        {
            std::vector<Ogre::Vector2> vertices(6);

            Ogre::Vector2 center = CellUtil::calculateCenter(x, y, offset, rad);

            float RAD = scale * 2 * rad / std::sqrt(3.0f);

            for (int i = 0; i < 6; i++)
            {
                float angle_rad = (60.0f * i + 30.0f) * Ogre::Math::PI / 180.0f;
                float dx = RAD * std::cos(angle_rad);
                float dy = RAD * std::sin(angle_rad);

                vertices[i] = Ogre::Vector2(center.x + dx, center.y + dy);
            }

            return vertices;
        }
    };
}; // end of namespace