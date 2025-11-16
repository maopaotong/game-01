#pragma once
#include "OgreMovableObject.h"
// marker interface

namespace fog
{
using namespace Ogre;
class Pickable
{
public:
    virtual bool pickUp(MovableObject *) = 0;
};
};