#pragma once
#include "State.h"
#include "util/Polygon2.h"
#include "util/CellUtil.h"
#include <Ogre.h>
#include <OgreVector.h>
namespace fog
{
    using namespace Ogre;

    class Ground
    {
        typedef float (*HeightFunction)(float x, float y, float heightOffset);

    public:
    public:
        class Transfer
        {
        public:
            static const bool VERTEX_ORDER_REVERSE = false; // is look from back

            static Vector3 to3D(Vector2 &vec2, HeightFunction hF = defaultHeightFunction, float heightOffset = 0.0f)
            {
                return to3D(vec2.x, vec2.y, hF, heightOffset);
            }

            static Vector3 to3D(float x, float y, HeightFunction hF = defaultHeightFunction, float heightOffset = 0.0f)
            {
                float height = hF(x, y, heightOffset);
                if (VERTEX_ORDER_REVERSE)
                {
                    return Vector3(y, height, -x);
                }
                else
                {
                    return Vector3(x, height, -y);
                }
            }

            static Vector2 to2D(const Vector3 &vec3, float &height)
            {
                height = vec3.y;
                return to2D(vec3);
            }
            static Vector2 to2D(const Vector3 &vec3)
            {
                if (VERTEX_ORDER_REVERSE)
                {

                    return Vector2(-vec3.z, vec3.x);
                }
                else
                {

                    return Vector2(vec3.x, -vec3.z);
                }
            }

            template <typename... Args>
            static void to3D(Vector2 &vec2, Vector3 &vec3, HeightFunction hF = defaultHeightFunction, float heightOffset = 0.0f)
            {
                vec3 = to3D(vec2, hF, heightOffset);
            }

            template <typename... Args>
            static std::vector<Ogre::Vector3> to3D(std::vector<Ogre::Vector2> &vec2Vec, HeightFunction hF = defaultHeightFunction, float heightOffset = 0.0f)
            {
                std::vector<Ogre::Vector3> vertices(vec2Vec.size());

                for (int i = 0; i < vec2Vec.size(); i++)
                {
                    to3D(vec2Vec[VERTEX_ORDER_REVERSE ? (6 - i - 1) : i], vertices[i], hF, heightOffset);
                }

                return vertices;
            }
        };

    public:
        static inline const Vector3 DEFAULT_FORWARD = Ogre::Vector3::UNIT_Z;
        static inline const Vector3 DIRECTION_DOWN = Ogre::Vector3::NEGATIVE_UNIT_Y;
        static Quaternion getRotationTo(Vector2 &direction)
        {
            Vector3 d3 = Transfer::to3D(direction);
            return DEFAULT_FORWARD.getRotationTo(d3);
        }

        template <typename... Args>
        static std::vector<Ogre::Vector3> calculateVertices3D(int x, int y, CostMap *costMap, float rad, float scale = 1.0f, HeightFunction hF = defaultHeightFunction, float heightOffset = 0.0f)
        {
            Vector2 offset = CellUtil::offset(costMap);
            return calculateVertices3D(x, y, offset, rad, scale, hF, heightOffset);
        }

        template <typename... Args>
        static std::vector<Ogre::Vector3> calculateVertices3D(int x, int y, Vector2 &offset, float rad, float scale = 1.0f, HeightFunction hF = defaultHeightFunction, float heightOffset = 0.0f)
        {
            std::vector<Ogre::Vector2> vec2Vec = CellUtil::calculateVertices(x, y, offset, rad, scale);
            return Transfer::to3D(vec2Vec, hF, heightOffset);
        }

        static float defaultHeightFunction(float x, float y, float heightOffset)
        {
            return heightOffset;
        }

    public:
        Ground();
        virtual bool isPointInside(float x, float z) = 0;
        virtual bool isPointInside(Vector2 &p) = 0;
    }; // end of class

    typedef std::vector<std::vector<Vector3>> TerrainedVertices3;

    class TerrainedGround
    {
    public:
        static TerrainedVertices3 calculateVertices3D(int cellX, int cellY, CostMap *costMap, float rad, float heightOffset = 0.0f)
        {
            Vector2 cellCenterIn2DWld = CellUtil::calculateCenter(cellX, cellY, costMap, rad);
            
            float cellWidth = rad / std::sqrt(3);
            float cellHeight = rad * 2.0f;

            // calculate the rectangle scope.
            float cllMinXIn2DWld = cellCenterIn2DWld.x - cellWidth / 2.0f;
            float cllMinYIn2DWld = cellCenterIn2DWld.y - cellHeight / 2.0f;
            float cllMaxXIn2DWld = cellCenterIn2DWld.x + cellWidth / 2.0f;
            float cllMaxYIn2DWld = cellCenterIn2DWld.y + cellHeight / 2.0f;
            
            //
            
        }
    };

    class TerrainedGround_TODO
    {
    public:
        static TerrainedVertices3 calculateVertices3D(int cellX, int cellY, CostMap *costMap, float rad, float heightOffset = 0.0f)
        {
            // center in world 2D
            Vector2 cellCenterIn2DWld = CellUtil::calculateCenter(cellX, cellY, costMap, rad); //

            // center
            float cellWidth = rad * 2.0f;
            float cellHeight = rad / std::sqrt(3);

            // calculate the rectangle scope.
            float cllMinXIn2DWld = cellCenterIn2DWld.x - cellWidth / 2.0f;
            float cllMinYIn2DWld = cellCenterIn2DWld.y - cellHeight / 2.0f;
            float cllMaxXIn2DWld = cellCenterIn2DWld.x + cellWidth / 2.0f;
            float cllMaxYIn2DWld = cellCenterIn2DWld.y + cellHeight / 2.0f;

            Terrains *terrains = Context<Terrains *>::get();
            // dencity of terrain
            float density = terrains->getDensity(); //
            // scope of index of the height map
            Vector3 terOrigin = terrains->getOrigin();
            // transfer terrain origin position to world 2D
            Vector2 terOriginIn2DWld = Ground::Transfer::to2D(terOrigin);

            // scaler
            int tMinX = (int)((cllMinXIn2DWld - terOriginIn2DWld.x) / density);
            int tMinY = (int)((cllMinYIn2DWld - terOriginIn2DWld.y) / density);
            int tMaxX = (int)((cllMaxXIn2DWld - terOriginIn2DWld.x) / density);
            int tMaxY = (int)((cllMaxYIn2DWld - terOriginIn2DWld.y) / density);

            // make the result
            TerrainedVertices3 ret;
            for (int y = tMinY; y < tMaxY; y++)
            {

                std::vector<Vector3> row;
                for (int x = tMinX; x < tMaxX; x++)
                {
                    bool addAllCell = true;

                    ;
                    Vector2 pointIn2DWld = cellCenterIn2DWld + Vector2(x * density, y * density);

                    if (addAllCell || CellUtil::isPointInCell(pointIn2DWld.x, pointIn2DWld.y, cellX, cellY, costMap))
                    {
                        Vector3 pos = Ground::Transfer::to3D(pointIn2DWld, Global::getTerrainHeightAtPositionWithOffset, heightOffset);
                        row.push_back(pos);
                        std::cout << "push cell(x:" << cellX << ",y:" << cellY << "),pos:" << pos << std::endl;
                    }
                }

                if (!row.empty())
                {
                    ret.push_back(row);
                }
            }

            return ret;
        }

        static bool isInsideOfCell_TODO(Vector2 center, Vector2 point, float rad)
        {
            const float SQRT3 = 1.73205080757f;

            Vector2 pointNom = point - center; //
            float dx = pointNom.x;
            float dy = pointNom.y;
            float R = rad * SQRT3;
            // Pointy-top hexagon (vertices at top/bottom)
            // 条件：|dx| <= √3 * R / 2 且 |dz| <= R 且 |√3*dx ± dz| <= √3 * R
            float a = SQRT3 * dx + dy;
            float b = SQRT3 * dx - dy;
            return (std::fabs(dx) <= SQRT3 * R / 2.0f) &&
                   (std::fabs(dy) <= R) &&
                   (std::fabs(a) <= SQRT3 * R) &&
                   (std::fabs(b) <= SQRT3 * R);
        }

    }; // end of class
}; // end of namespace