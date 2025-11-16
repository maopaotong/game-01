#pragma once
#include <Ogre.h>
#include "fg/util/CellMark.h"
namespace fog
{

    class Actor
    {
    public:
        virtual ~Actor() = default;
        virtual CellKey  getDestinationCell() = 0;
    };
};