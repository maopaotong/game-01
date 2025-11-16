#pragma once
#include <Ogre.h>
#include <OgreVector.h>


namespace fog
{
using namespace Ogre;

class Terrains
{
public:
    virtual ~Terrains() {};

    float getHeightAtPosition(Vector2 &pos)
    {
        return getHeightAtPosition(pos.x, pos.y);
    }

    virtual float getHeightAtPosition(float x, float y) = 0;
};
};