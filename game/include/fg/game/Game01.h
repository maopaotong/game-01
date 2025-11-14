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

class Game01 : public Module, public ImGuiApp::FrameListener
{
    Global *global;
    Core *core;

public:
    Game01()
    {
    }
    std::string getName() override
    {
        return "example.costMapMod";
    }

    void active(Core *core) override
    {
        this->core = core;
        this->initGlobalVarPtr(core->getGlobal());
        CostMap *costMap = createCostMap();
        // Create materials before buding mesh?
        MaterialFactory::createMaterials(core->getMaterialManager());
        Ground *ground = new CostMapGround(costMap);
        State *world = new WorldStateControl(costMap, ground, core);
        SceneNode *node = core->getSceneManager()->getRootSceneNode();
        world->setSceneNode(node);
        this->global = core->getGlobal();
        core->getImGuiApp()->addFrameListener(this);
    }

    void onFrame(const FrameEvent &evt)
    {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));

        ImGui::Begin("Hello");
        Actor *actor = global->getActiveActor();
        if (actor)
        {
            ImGui::Button(":Right click a cell as the destination!");
        }
        else
        {
            ImGui::Button(":Left click to pick an actor!");
        }
        Viewport *vp = core->getViewport();
        RenderWindow *window = core->getWindow();

        ImGui::Text(fmt::format("Viewport.norm:{},{},{},{}", vp->getLeft(), vp->getTop(), vp->getWidth(), vp->getHeight()).c_str());
        ImGui::Text(fmt::format("Viewport.pixel:{},{},{},{}", vp->getActualLeft(), vp->getActualTop(), vp->getActualWidth(), vp->getActualHeight()).c_str());
        ImGui::Text(fmt::format("Window.pixel:{},{}", window->getWidth(), window->getHeight()).c_str());

        global->forEachVarPtr([](std::string name, float *vPtr, Global::VarScope<float> *scope)
                              { 
                            float min  = scope?scope->min:0;
                            float max = scope?scope->max:100;
                            ImGui::SliderFloat(name.c_str(), vPtr, min, max); });

        ImGui::End();
        ImGui::PopStyleColor();
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
    void initGlobalVarPtr(Global *glb)
    {
        glb->setVarAndScope(".aniSpeed", 0.55f, 0.0f, 2.0f);
        glb->setVarAndScope(".pathSpeed", 1.0f, 1.0f, 10.0f);
    }
};