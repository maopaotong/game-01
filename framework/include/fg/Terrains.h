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

        virtual float getHeightAtPosition(Vector3 &pos) = 0;

        virtual Vector3 getOrigin() = 0;

    };
};