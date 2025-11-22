#pragma once
#include <Ogre.h>
#include <OgreInput.h>
#include "Core.h"

namespace fog
{

    using namespace Ogre;
    using namespace OgreBites;
    class Module
    {
    public:
        class Active
        {
        };
        class Deactive
        {
        };

    protected:
    public:
        virtual std::string getName() = 0;
        virtual void active() = 0;
        virtual void disactive() = 0;
    };
};