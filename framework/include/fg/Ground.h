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
};
};