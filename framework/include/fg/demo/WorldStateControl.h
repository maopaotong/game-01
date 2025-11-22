
#pragma once
#include <iostream>
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/util/CostMap.h"
#include "fg/InputState.h"
#include <unordered_map>
#include "fg/util/CellUtil.h"
#include "fg/State.h"
#include "fg/core/MainInputListener.h"
#include "fg/demo/ActorStateControl.h"
#include "fg/demo/CellStateControl.h"
#include "fg/demo/CellMarkStateControl.h"
#include "fg/State.h"
#include "fg/Core.h"
#include "fg/core/CameraState.h"
#include "fg/WorldState.h"
#include "fg/core/SimpleInputState.h"
#include "fg/core/MouseClickPicker.h"
#include "fg/Terrains.h"
#include "fg/demo/ActorSelectControl.h"
#include "fg/demo/KeepWorld.h"
#include "fg/Targets.h"

namespace fog
{
    using namespace Ogre;
    // root state & control.
    class WorldStateControl : public WorldState
    {
    protected:
        CellStateControl *cells;

        CostMap *costMap;

        std::unordered_map<MarkType, CellMarkStateControl *> markStateControls;

        SimpleInputState *inputState;
        Core *core;

    public:
        class KeepWorldOwner : public KeepWorld::Owner
        {
        public:
            KeepWorldOwner(CostMap *costMap, Core *core, State *state) : KeepWorld::Owner(costMap, core, state)
            {
            }
            bool tryTakeTarget(Tasks::Target *target) override
            {
                this->pushOrWait(new KeepWorld::Task(dynamic_cast<Targets::KeepWorld *>(target), this, state));
                return true;
            }
        };
        WorldStateControl(CostMap *costMap, Ground *ground, Core *core) : costMap(costMap), core(core), WorldState(ground)
        {

            Ogre::Root *root = core->getRoot();

            // Create frame listener for main loop
            this->cells = new CellStateControl(costMap, core);
            this->cells->init();
            this->addChild(this->cells);

            this->inputState = new SimpleInputState(core->getCamera(), core->getWindow());

            CameraState *cameraState = new CameraState(core->getCamera(), inputState);
            cameraState->setGround(this->ground); //
            root->addFrameListener(cameraState);

            markStateControls[MarkType::ACTIVE] = new CellMarkStateControl(costMap, core, MarkType::ACTIVE);
            ActorStateControl *actor1 = new ActorStateControl("actor1", costMap, core);
            actor1->init();
            this->addChild(actor1);

            ActorStateControl *actor2 = new ActorStateControl("actor2", costMap, core);
            actor2->init();
            this->addChild(actor2);

            //

            MainInputListener *keyHandler = new MainInputListener(costMap, core);
            core->getAppContext()->addInputListener(keyHandler);
            core->getAppContext()->addInputListener(inputState);
            core->getAppContext()->addInputListener(new MouseClickPicker(core->getCamera(), core->getSceneManager(), core->getViewport()));

            // task owner
            KeepWorld::Owner *owner = new KeepWorldOwner(costMap, core, this);

            this->taskOwner = owner;
        }

        virtual void init() override
        {
            Targets::KeepWorld *target = new Targets::KeepWorld();

            this->taskOwner->tryTakeTarget(target);
        }

        CostMap *getCostMap()
        {
            return costMap;
        }
    };
}; // end of namespace