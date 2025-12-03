#pragma once
#include <Ogre.h>
#include <OgreVector.h>
#include "fg/util/CellMark.h"

using namespace Ogre;

namespace fog
{

    struct Height
    {
        // Terrains *terrains;
        // Height(Terrains *terrains) : terrains(terrains) {}
        float operator()(Vector3 &pos, Vector3 *pNorm)
        {
            //return terrains->getHeightWithNormalAtWorldPosition(pos, pNorm);
            return 0.0f;
        }
    };

    class Plane
    {

        Height height;

        Vector3 origin;

        Vector3 forwardDirection = Vector3::UNIT_Z;

    public:
        Plane() : height(Height()), origin(Vector3(0, 0, 0))
        {
        }
        Vector3 getOrigin()
        {
            return origin;
        }
        void setOrigin(Vector3 origin)
        {
            this->origin = origin;
        }

        Vector3 to3DInPlane(Vector2 point){
            Vector3 ret = Vector3(point.x, 0, -point.y);
            return ret;
        }

        Vector3 to3D(Vector2 point, Vector3 *norm)
        {
            Vector3 ret = to3DInPlane(point);
            ret = ret + origin;

            ret.y = height(ret, norm);
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
        Quaternion getRotationTo(Vector2 direction2D)
        {
            Vector3 v3 = to3D(direction2D, nullptr);
            return forwardDirection.getRotationTo(v3);
        }
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

        float getScale()
        {
            return this->scale;
        }

        Vector3 to3D(Vector2 cellOrigin, Vector2 pointInCell, Vector3 *norm)
        {
            Vector2 pointIn2D = pointInCell * this->scale + cellOrigin;
            Vector3 positionIn3D = this->plane->to3D(pointIn2D, norm);
            return positionIn3D;
        }

        Vector3 to3D(Vector2 p2D)
        {
            return to3D(p2D, nullptr);
        }
        Vector3 to3D(Vector2 p2D, Vector3 *norm)
        {
            return this->plane->to3D(p2D, norm);
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