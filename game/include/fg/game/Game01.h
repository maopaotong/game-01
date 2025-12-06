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
#include "fg/core/Tiles.h"
#include "fg/Config.h"

namespace fog
{
    class Game01 : public Mod, public FrameListener, CoreMod::Callback
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

        // before ogre load material, create textures.
        void beforeResourceLoad() override
        {
        }
        void afterResourceLoad() override
        {
            std::string tName = "TerrainsTex001";
            Context<Tiles::Terrains>::get()->createWorldTexture(tName);
            MaterialPtr mat = MaterialManager::getSingletonPtr()->getByName("Tiles");

            mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(tName);
        }

        void setup() override
        {

            Config::init("game/config/fog.cfg");
            Context<CoreMod>::get()->addCallback(this);

            CostMap *costMap = createCostMap(Config::TILES_WIDTH, Config::TILES_WIDTH);
            Context<CostMap>::set(costMap);

            fog::Plane *p = new fog::Plane();
            Context<Plane>::set(p);

            {
                Node2D *root2D = new Node2D(p, Config::CELL_SCALE); //
                Context<Node2D>::set(root2D);
            }
            {

                Cell::Center *cells = new Cell::Center(Context<Node2D>::get());
                cells->translateToCenter();
                Context<Cell::Center>::set(cells);
            }

            int tWidth = Context<Cell::Center>::get()->getWidth();
            int tHeight = Context<Cell::Center>::get()->getHeight();

            std::vector<std::vector<Tiles::Tile>> tiles(tWidth, std::vector<Tiles::Tile>(tHeight, Tiles::Tile()));
            Tiles::Generator::generateTiles(tiles, tWidth, tHeight);

            int qWidth = tWidth * Config::TILE_TERRAIN_QUALITY;
            int qHeight = tHeight * Config::TILE_TERRAIN_QUALITY * std::sqrt(3) / 2.0f;
            Tiles::Terrains *terrains = new Tiles::Terrains(qWidth, qHeight);
            terrains->init(tiles, tWidth, tHeight);
            Context<Tiles::Terrains>::set(terrains);
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

            // root2D->position = -cells->getCenterIn2D(); // move center to (0,0)

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
        CostMap *createCostMap(int width, int height)
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
