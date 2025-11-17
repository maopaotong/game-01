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

    public:
        Plane(Terrains *terrains) : height(Height(terrains))
        {
        }

        Vector3 to3D(Vector2 &point)
        {
            Vector3 ret = Vector3(point.x, 0, -point.y);
            ret.y = height(ret);
            return ret;
        };

        Vector2 to2D(Vector3 &position, float &height)
        {
            height = position.y;
            return to2D(position);
        }

        Vector2 to2D(Vector3 &position)
        {
            Vector2 ret = Vector2(position.x, -position.z);
            return ret;
        };
    };

    struct Node2D
    {

        float scale = 1.0;
        Vector2 position;
        Plane *plane;
        Node2D(Plane *p, float scale) : plane(p), scale(scale), position((0.0f, 0.0f))
        {
        }

        Vector2 to2D(Vector3 pos)
        {
            Vector ret = this->plane->to2D(pos);
            ret = ret - position;
            return ret;
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