
#pragma once
#include <iostream>
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "util/CostMap.h"
#include <unordered_map>
#include "util/CellUtil.h"
#include "State.h"
namespace fog
{
using namespace Ogre;
// root state & control.
class IWorld
{

public:
    virtual CostMap * getCostMap() = 0;
    
};
};