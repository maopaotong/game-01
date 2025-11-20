#pragma once
#include <Ogre.h>
#include "fg/util/CellMark.h"
#include "fg/Task.h"
namespace fog
{

    class Actor
    {
    public:
        virtual ~Actor() = default;
        virtual CellKey  getDestinationCell() = 0;
    };
};