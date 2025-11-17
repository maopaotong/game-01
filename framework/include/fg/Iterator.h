#pragma once
#include <Ogre.h>
#include <OgreVector.h>
#include "CellMark.h"

using namespace Ogre;

namespace fog
{
    template <typename T>
    class Iterator
    {
    public:
        template<typename... Args>
        virtual bool next(void (*)(T &ret, Args...)) = 0;
    };

};