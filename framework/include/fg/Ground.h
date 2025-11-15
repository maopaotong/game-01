#pragma once
#include "State.h"
#include "util/Polygon2.h"
#include "util/CellUtil.h"
#include <Ogre.h>
#include <OgreVector.h>
using namespace Ogre;

class Ground
{
public:
    class Transfer
    {
    public:
        static const bool VERTEX_ORDER_REVERSE = false; // is look from back

        static Vector3 to3D(Vector2 &vec2, float height = 0.0f)
        {
            return to3D(vec2.x, vec2.y, height);
        }

        static Vector3 to3D(float x, float y, float height = 0.0f)
        {
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

        static void to3D(Vector2 &vec2, Vector3 &vec3, float height = 0.0f)
        {
            vec3 = Transfer::to3D(vec2, height);
        }

        static std::vector<Ogre::Vector3> to3D(std::vector<Ogre::Vector2> &vec2Vec, float height = 0.0f)
        {
            std::vector<Ogre::Vector3> vertices(vec2Vec.size());

            for (int i = 0; i < vec2Vec.size(); i++)
            {
                to3D(vec2Vec[VERTEX_ORDER_REVERSE ? (6 - i - 1) : i], vertices[i], height);
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

    static std::vector<Ogre::Vector3> calculateVertices3D(int x, int y, CostMap *costMap, float rad, float scale = 1.0f)
    {
        Vector2 offset = CellUtil::offset(costMap);
        return calculateVertices3D(x, y, offset, rad, scale);
    }
    static std::vector<Ogre::Vector3> calculateVertices3D(int x, int y, Vector2 &offset, float rad, float scale = 1.0f)
    {
        std::vector<Ogre::Vector2> vec2Vec = CellUtil::calculateVertices(x, y, offset, rad, scale);
        return Transfer::to3D(vec2Vec);
    }

public:
    Ground();
    virtual bool isPointInside(float x, float z) = 0;
    virtual bool isPointInside(Vector2 &p) = 0;
};