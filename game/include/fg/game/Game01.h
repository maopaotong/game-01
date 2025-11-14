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

class Game01 : public Module
{
    Global *global;
    Core *core;
    bool breakRenderRequested = false;
    RenderWindow *window;
    Viewport *vp;
    SceneManager *sceMgr;
    GameTerrain *terrain = nullptr;

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
        delete this->terrain;
        this->terrain = nullptr;
    }

    void active(Core *core) override
    {
        this->core = core;
        this->global = core->getGlobal();
        this->window = core->getWindow();
        this->vp = core->getViewport();
        this->sceMgr = core->getSceneManager();

        CostMap *costMap = createCostMap();
        // Create materials before buding mesh?
        MaterialFactory::createMaterials(core->getMaterialManager());
        Ground *ground = new CostMapGround(costMap);
        State *world = new WorldStateControl(costMap, ground, core);

        SceneNode *node = sceMgr->getRootSceneNode();
        world->setSceneNode(node);

        //
        terrain = new GameTerrain();
        RenderSystem *rSys = core->getRoot()->getRenderSystem();
        Light *light = core->getLight();
        terrain->load(rSys, sceMgr, light);
    }

    CostMap *createCostMap()
    {
        int width = 30;
        int height = 25;
        CostMap *cm = new CostMap(60, 50);
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