#pragma once

#include <Ogre.h>
#include "fg/PathFollow2.h"
#include "fg/util/CellUtil.h"
#include "fg/util/CostMap.h"
#include "fg/State.h"
#include "PathState.h"
#include "fg/Pickable.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/util/CollectionUtil.h"
#include "fg/core/EntityState.h"
namespace fog
{
    using namespace Ogre;
    using Vector3Ref = Property::Ref<Vector3>;

    class H0085 : public EntityState
    {

        constexpr static float ACTOR_SCALE = 30.0f;
        constexpr static float ACTOR_HEIGHT = 10.0f;

    protected:
    public:
        H0085() : EntityState("h0085.mesh", ACTOR_SCALE, ACTOR_HEIGHT, {})
        {
            
        }

        virtual ~H0085()
        {
        }

        virtual void init() override
        {
            EntityState::init();
            //this->sceNode->setOrientation();
        }
    };
}; // end of namespace
