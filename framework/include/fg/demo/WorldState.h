#pragma once
#include "fg/State.h"
#include "CellStateControl.h"
#include "fg/core/SimpleInputState.h"
#include "fg/core/CameraState.h"
#include "fg/core/EntityState.h"
#include "fg/core/MainInputListener.h"
#include "fg/core/MouseClickPicker.h"
namespace fog
{
    class WorldState : public State
    {
    protected:
        CellStateControl *cells;

        SimpleInputState *inputState;

    public:
        WorldState()
        {
            CostMap *costMap = Context<CostMap *>::get();
            Core *core = Context<Core *>::get();
            Ogre::Root *root = core->getRoot();

            // Create frame listener for main loop
            this->cells = new CellStateControl();
            this->cells->init();
            this->addChild(this->cells);

            this->inputState = new SimpleInputState(core->getCamera(), core->getWindow());

            CameraState *cameraState = new CameraState(core->getCamera(), inputState);
            root->addFrameListener(cameraState);

            EntityState *actor1 = new EntityState("actor1");
            actor1->init();
            this->addChild(actor1);

            EntityState *actor2 = new EntityState("actor2");
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
};