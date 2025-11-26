#pragma once
#include "fg/State.h"
#include "CellStateControl.h"
#include "fg/core/CameraState.h"
#include "fg/core/EntityState.h"
#include "fg/core/MouseCameraController.h"
#include "fg/MovingStateManager.h"
#include "fg/CellInstanceManager.h"
#include "fg/demo/EntryController.h"
#include "fg/MovableStateManager.h"
#include "fg/BuildingStateManager.h"
#include "fg/InventoryStateManager.h"   
namespace fog
{
    class WorldState : public State
    {
    protected:
        CellStateControl *cells;

    public:
        WorldState()
        {
            CostMap *costMap = Context<CostMap>::get();
            CoreMod *core = Context<CoreMod>::get();
            Ogre::Root *root = core->getRoot();

            // Create frame listener for main loop
            this->cells = new CellStateControl();
            this->cells->init();
            this->addChild(this->cells);

            CameraState *cameraState = new CameraState();
            root->addFrameListener(cameraState);

            //
            MovableStateManager *movableStateMgr = Context<MovableStateManager >::get();
            movableStateMgr->init();
            this->addChild(movableStateMgr);
            //Context<MovableStateManager >::set(movableStateMgr);
            //           
            BuildingStateManager *buildingStateMgr = Context<BuildingStateManager >::get();
            buildingStateMgr->init();
            this->addChild(buildingStateMgr);
            //

            InventoryStateManager *inventoryStateMgr = Context<InventoryStateManager >::get();
            inventoryStateMgr->init();
            this->addChild(inventoryStateMgr);
            //

            MainInputListener *keyHandler = new MainInputListener(costMap);
            core->getAppContext()->addInputListener(keyHandler);

            EntryController *entryController = new EntryController();
            core->getAppContext()->addInputListener(entryController);
            root->addFrameListener(entryController);

        }

        virtual void init() override
        {
        }
    };
};