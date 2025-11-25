#pragma once
#include "fg/State.h"
#include "CellStateControl.h"
#include "fg/core/InputStateController.h"
#include "fg/core/CameraState.h"
#include "fg/core/EntityState.h"
#include "fg/core/MouseCameraController.h"
#include "fg/MovingStateManager.h"
#include "fg/CellInstanceManager.h"
#include "fg/demo/EntryController.h"
#include "fg/MovableStateManager.h"
#include "fg/core/MouseClickPicker.h"
namespace fog
{
    class WorldState : public State
    {
    protected:
        CellStateControl *cells;

        InputStateController *inputState;

    public:
        WorldState()
        {
            CostMap *costMap = Context<CostMap *>::get();
            CoreMod *core = Context<CoreMod *>::get();
            Ogre::Root *root = core->getRoot();

            // Create frame listener for main loop
            this->cells = new CellStateControl();
            this->cells->init();
            this->addChild(this->cells);

            this->inputState = new InputStateController(core->getCamera(), core->getWindow());
            CameraState *cameraState = new CameraState(core->getCamera(), inputState);
            root->addFrameListener(cameraState);

            MovableStateManager *movableStateMgr = new MovableStateManager();
            movableStateMgr->init();
            this->addChild(movableStateMgr);
            Context<MovableStateManager *>::set(movableStateMgr);
            //           

            MainInputListener *keyHandler = new MainInputListener(costMap);
            core->getAppContext()->addInputListener(keyHandler);
            core->getAppContext()->addInputListener(inputState);

            EntryController *entryController = new EntryController();
            core->getAppContext()->addInputListener(entryController);
            root->addFrameListener(entryController);


            core->getAppContext()->addInputListener(new MouseClickPicker(this, core->getCamera(), core->getSceneManager(), core->getViewport()));
        }

        virtual void init() override
        {
        }
    };
};