
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

namespace fog
{
    using namespace Ogre;
    // root state & control.
    class WorldStateControl : public WorldState
    {
    protected:
        CellStateControl *cells;

        std::unordered_map<MarkType, CellMarkStateControl *> markStateControls;

        SimpleInputState *inputState;
        

    public:
        WorldStateControl(Ground *ground) : WorldState(ground)
        {
            CostMap *costMap = Context<CostMap *>::get();
            Core* core = Context<Core*>::get();
            Ogre::Root *root = core->getRoot();

            // Create frame listener for main loop
            this->cells = new CellStateControl();
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

            MainInputListener *keyHandler = new MainInputListener(costMap);
            core->getAppContext()->addInputListener(keyHandler);
            core->getAppContext()->addInputListener(inputState);
            
            core->getAppContext()->addInputListener(new MouseClickPicker(this, core->getCamera(), core->getSceneManager(), core->getViewport()));
        }

        virtual void init() override
        {
        }

    };
}; // end of namespace