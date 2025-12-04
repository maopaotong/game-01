#pragma once
#include "fg/CoreMod.h"
#include "fg/Mod.h"
#include "fg/util/CostMap.h"
#include "fg/core/MaterialFactory.h"
#include "fg/demo/WorldState.h"

#include "imgui.h"
#include <fmt/format.h>
#include <random>
#include <OgreRenderWindow.h>
#include "fg/demo/GameTerrain.h"
#include "fg/ui/OnFrameUI.h"
#include "fg/State.h"
#include "fg/Options.h"
#include "fg/TaskRunner.h"
namespace fog
{
    class Game01 : public Mod, public FrameListener
    {

        int width = 256 + 1;
        int height = width;

        bool breakRenderRequested = false;
        RenderWindow *window;
        Viewport *vp;
        SceneManager *sceMgr;
        OnFrameUI *onFrameUI = nullptr;

    public:
        Game01()
        {
        }
        virtual ~Game01()
        {
        }
        std::string getName() override
        {
            return "example.costMapMod";
        }
        void deactive() override
        {

            delete Context<Terrains>::unset();
            delete Context<CostMap>::unset();
            delete this->onFrameUI;
        }

        void active() override
        {
            CoreMod *core = Context<CoreMod>::get();
            this->window = core->getWindow();
            this->vp = core->getViewport();
            this->sceMgr = core->getSceneManager();
            CostMap *costMap = createCostMap();
            Context<CostMap>::set(costMap);

            this->onFrameUI = new OnFrameUI();
            Context<CoreMod>::get()->getImGuiApp()->addFrameListener(this->onFrameUI);
            //
            //

            // Create materials before buding mesh?
            MaterialFactory::createMaterials(core->getMaterialManager());
            //
            RenderSystem *rSys = core->getRoot()->getRenderSystem();
            Light *light = core->getLight();
            // GameTerrains *terrains = new GameTerrains();
            // terrains->load(rSys, sceMgr, light);
            // Context<Terrains>::set(terrains);
            // //
            fog::Plane *p = new fog::Plane();
            Context<Plane>::set(p);

            float scale = 30.0f;
            Node2D *root2D = new Node2D(p, scale); //

            Context<Node2D>::set(root2D);

            Cell::Center *cells = new Cell::Center(root2D);
            cells->translateToCenter();

            // root2D->position = -cells->getCenterIn2D(); // move center to (0,0)
            Context<Cell::Center>::set(cells);
            // Ground *ground = new CostMapGround(costMap);

            //
            State *world = new WorldState();
            SceneNode *node = sceMgr->getRootSceneNode();
            world->setSceneNode(node);
            Context<State>::set(world);

            //
            CellInstanceManager *cellInstances = Context<CellInstanceManager>::get();
            cellInstances->init();

            // moving state manager.
            MovingStateManager *movingStateManager = Context<MovingStateManager>::get();
            movingStateManager->init();
            //
            Context<CoreMod>::get()->addStepListener(movingStateManager);

            world->init();
            core->addFrameListener(this);
        }

        virtual bool frameStarted(const FrameEvent &evt)
        {
            // Context<State>::get()->forEach([&evt](State *state)
            //                                  {
            //                                      state->getSlots().step(evt.timeSinceLastFrame);

            //                                      return true; //
            //                                  });              //
            return true;
        }
        CostMap *createCostMap()
        {
            
            CostMap *cm = new CostMap(width, height);
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_int_distribution<int> rPosX(0, width - 1);  //
            std::uniform_int_distribution<int> rPosY(0, height - 1); //
            std::uniform_int_distribution<int> rCost(0, 3);          //

            for (int i = 0; i < (width * height) / 10; i++)
            {
                cm->setCost(rPosX(gen), rPosY(gen), rCost(gen));
            }

            return cm;
        }
    };
};
