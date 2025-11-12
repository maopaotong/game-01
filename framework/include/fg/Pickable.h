#pragma once
#include "OgreMovableObject.h"
// marker interface
using namespace Ogre;
class Pickable
{
public:
    virtual bool pickUp(MovableObject *) = 0;
};
