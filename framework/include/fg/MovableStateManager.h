#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/core/EntityState.h"
namespace fog
{
    
    class MovableStateManager : public State
    {

    public:
        MovableStateManager()
        {
        }
        virtual ~MovableStateManager()
        {
        }
        void init() override
        {
            EntityState *actor1 = new EntityState("actor1");
            actor1->init();
            this->addChild(actor1);

            EntityState *actor2 = new EntityState("actor2");
            actor2->init();
            this->addChild(actor2);
        }

    }; // end of class
}; // end of namespace