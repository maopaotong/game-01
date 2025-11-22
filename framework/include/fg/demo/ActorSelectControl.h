#pragma once
#include <Ogre.h>
#include <OgreSceneManager.h>
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/core/ActorState.h"
#include "fg/Core.h"
#include "fg/Terrains.h"
#include "fg/core/TrackActorByCell.h"

namespace fog
{
    using namespace Ogre;
    using Vector3Ref = Property::Ref<Vector3>;
    /**
     *
     * Tracking the active actor?
     * High light the cell if any actor is active on the cell.
     *
     */
    class ActorSelectControl : public CellStateBase
    {
    public:
    protected:
        CostMap *costMap;
        Core *core;
        Cell::Instance cell;

        bool active = false;
        Vector3Ref actorPosition;
        Vector3 prePosition;

    public:
        ActorSelectControl(CostMap *costMap, Core *core) : CellStateBase(core), costMap(costMap)
        {
        }

        void init() override
        {
            CellStateBase::init();
        }
        void rebuildMesh() override
        {
        }
    };
}; // end of namespace