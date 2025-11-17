#pragma once
#include <Ogre.h>
#include <OgreVector.h>
#include "fg/util/CellMark.h"

using namespace Ogre;

namespace fog
{
    struct Height
    {
        Terrains *terrains;
        Height(Terrains *terrains) : terrains(terrains) {}
        float operator()(Vector3 &pos)
        {
            return terrains->getHeightAtPosition(pos);
        }
    };

    class Plane
    {
        Height height;

        Vector3 origin;

    public:
        Plane(Terrains *terrains) : height(Height(terrains)), origin(Vector3(0, 0, 0))
        {
        }
        Vector3 getOrigin()
        {
            return origin;
        }
        void setOrigin(Vector3 origin)
        {
            //this->origin = origin;
        }

        Vector3 to3D(Vector2 point, float heightOffset = 0.0f)
        {
            Vector3 ret = Vector3(point.x, 0, -point.y);
            ret = ret + origin;
            ret.y = height(ret) + heightOffset;
            return ret;
        };

        Vector2 to2D(Vector3 position, float &height)
        {

            height = position.y - origin.y;
            return to2D(position);
        }

        Vector2 to2D(Vector3 position)
        {

            Vector2 ret = Vector2(position.x - origin.x, -(position.z - origin.z));
            return ret;
        };
    };

    struct Node2D
    {

        float scale = 1.0;
        Plane *plane;
        Node2D(Plane *p, float scale) : plane(p), scale(scale)
        {
        }

        Vector2 to2D(Vector3 pos)
        {
            Vector2 ret = this->plane->to2D(pos);
            return ret;
        }

        Vector3 to3D(Vector2 p2D, float heightOffset = 0.0f)
        {
            return this->plane->to3D(p2D, heightOffset);
        }
    };

    struct Object2D
    {
        Node2D *node;
        Object2D() : node(nullptr)
        {
        }
        Object2D(Node2D *node) : node(node)
        {
        }
    };

};