#pragma once
#include "fg/Global.h"
#include "fg/Core.h"
#include "fg/Module.h"
#include "fg/util/CostMap.h"
#include "fg/Ground.h"
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
    class Game01 : public Module, public FrameListener
    {
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
        void disactive() override
        {

            delete Context<Terrains *>::unset();
            delete Context<CostMap *>::unset();
            delete this->onFrameUI;
        }

        void active() override
        {
            Core *core = Context<Core *>::get();
            this->window = core->getWindow();
            this->vp = core->getViewport();
            this->sceMgr = core->getSceneManager();
            CostMap *costMap = createCostMap();
            Context<CostMap *>::set(costMap);

            this->onFrameUI = new OnFrameUI();
            Context<Core *>::get()->getImGuiApp()->addFrameListener(this->onFrameUI);
            //
            //

            // Create materials before buding mesh?
            MaterialFactory::createMaterials(core->getMaterialManager());
            //
            GameTerrains *terrains = new GameTerrains();
            RenderSystem *rSys = core->getRoot()->getRenderSystem();
            Light *light = core->getLight();
            terrains->load(rSys, sceMgr, light);
            Context<Terrains *>::set(terrains);
            //
            fog::Plane *p = new fog::Plane(terrains);
            Context<Plane*>::set(p);

            float scale = 30.0f;
            Node2D *root2D = new Node2D(p, scale); //

            Context<Node2D *>::set(root2D);

            Cell::Center *cells = new Cell::Center(root2D);
            cells->translateToCenter();

            // root2D->position = -cells->getCenterIn2D(); // move center to (0,0)
            Context<Cell::Center *>::set(cells);
            // Ground *ground = new CostMapGround(costMap);

            //
            State *world = new WorldState();
            SceneNode *node = sceMgr->getRootSceneNode();
            world->setSceneNode(node);
            Context<State *>::set(world);

            world->init();
            core->addFrameListener(this);
        }

        virtual bool frameStarted(const FrameEvent &evt)
        {
            Context<State *>::get()->forEach([&evt](State *state)
                                             {
                                                 state->getTaskRunner()->step(evt.timeSinceLastFrame);

                                                 return true; //
                                             });              //
            return true;
        }
        CostMap *createCostMap()
        {
            int width = 30;
            int height = 25;
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
