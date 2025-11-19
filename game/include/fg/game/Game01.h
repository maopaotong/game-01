#pragma once
#include "fg/Global.h"
#include "fg/Core.h"
#include "fg/Module.h"
#include "fg/util/CostMap.h"
#include "fg/Ground.h"
#include "fg/core/MaterialFactory.h"
#include "fg/core/CostMapGround.h"
#include "fg/demo/WorldStateControl.h"

#include "imgui.h"
#include <fmt/format.h>
#include <random>
#include <OgreRenderWindow.h>
#include "fg/demo/GameTerrain.h"
#include "fg/ui/OnFrameUI.h"
#include "fg/Game.h"
#include "fg/State.h"
#include "fg/Options.h"
namespace fog
{
    class Game01 : public Module, Game
    {
        Global *global;
        Core *core;
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
            delete Global::Context<Terrains *>::unset();
        }

        void active(Core *core) override
        {
            this->core = core;
            this->global = core->getGlobal();
            this->window = core->getWindow();
            this->vp = core->getViewport();
            this->sceMgr = core->getSceneManager();
            CostMap *costMap = createCostMap();

            this->onFrameUI = new OnFrameUI(core, costMap);
            this->core->getImGuiApp()->addFrameListener(this->onFrameUI);
            //
            //

            // Create materials before buding mesh?
            MaterialFactory::createMaterials(core->getMaterialManager());
            //
            GameTerrains *terrains = new GameTerrains();
            RenderSystem *rSys = core->getRoot()->getRenderSystem();
            Light *light = core->getLight();
            terrains->load(rSys, sceMgr, light);
            Global::Context<Terrains *>::set(terrains);
            //
            fog::Plane *p = new fog::Plane(terrains);

            float scale = 30.0f;
            Node2D *root2D = new Node2D(p, scale); //

            Global::Context<Node2D *>::set(root2D);

            Cell::Center *cells = new Cell::Center(root2D, costMap);
            cells->translateToCenter();

            // root2D->position = -cells->getCenterIn2D(); // move center to (0,0)
            Global::Context<Cell::Center *>::set(cells);

            //
            Ground *ground = new CostMapGround(costMap);
            State *world = new WorldStateControl(costMap, ground, core);
            SceneNode *node = sceMgr->getRootSceneNode();
            world->setSceneNode(node);

            world->init();

            //
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

        void start(Options *options) override
        {
            // rebuild cells
            void (*func)(State *, Options *) = [](State *state, Options *options)
            {
                CellStateControl *csc = dynamic_cast<CellStateControl *>(state);

                if (csc)
                {
                    bool &showPlainCellPtr = options->getOption("Show-plain-cell?")->getValueRef<bool>();
                    csc->showCost1 = showPlainCellPtr;
                    std::cout << "" << "" <<std:: endl;
                    csc->rebuildMesh();
                }
            };
            core->getRootState()->forEachChild<Options*>(true, func, options);
        }
    };
};
