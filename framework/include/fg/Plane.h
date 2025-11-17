#pragma once
#include <Ogre.h>
#include <OgreVector.h>
#include "fg/util/CellMark.h"

using namespace Ogre;

namespace fog
{

    class Plane
    {
    public:
        Vector3 to3D(Vector2 &point, float height)
        {
            return Vector3(point.x, height, -point.y);
        };
        Vector2 to2D(Vector3 &position, float &height)
        {
            height = position.y;
            return Vector2(position.x, -position.z);
        };
    };

    struct Node2D
    {

        float scale;
        Vector2 position;
        Plane *plane;
    };

};