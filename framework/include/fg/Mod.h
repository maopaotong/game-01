#pragma once
#include <Ogre.h>
#include <OgreInput.h>

namespace fog
{

    using namespace Ogre;
    using namespace OgreBites;
    class Mod
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
        virtual void deactive() = 0;
    };
};