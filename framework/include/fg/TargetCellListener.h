
#pragma once
#include <vector>
#include <Ogre.h>

namespace fog
{
class TargetCellListener
{
    
public:
    virtual bool onTargetCell(){
        return false;
    }
};
};