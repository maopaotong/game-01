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

class Game01 : public Module, public ImGuiApp::FrameListener
{
    Global *global;
    Core *core;
    bool breakRenderRequested = false;
    RenderWindow *window;
    Viewport *vp;
    SceneManager* sceMgr;
    GameTerrain *terrain;

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
        this->window = core->getWindow();
        this->vp = core->getViewport();
        this->sceMgr = core->getSceneManager();

        this->initGlobalVarPtr(core->getGlobal());
        CostMap *costMap = createCostMap();
        // Create materials before buding mesh?
        MaterialFactory::createMaterials(core->getMaterialManager());
        Ground *ground = new CostMapGround(costMap);
        State *world = new WorldStateControl(costMap, ground, core);

        //
        /*
        terrain = new GameTerrain();
        RenderSystem* rSys = core->getRoot()->getRenderSystem();
        Light * light = core->getLight();
        terrain->load(rSys, sceMgr, light);
        */

        SceneNode *node = sceMgr->getRootSceneNode();
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
        vp = core->getViewport();      

        ImGui::Text(fmt::format("Viewport.norm:     {},{},{},{}", vp->getLeft(), vp->getTop(), vp->getWidth(), vp->getHeight()).c_str());
        ImGui::Text(fmt::format("Viewport.pixel:    {},{},{},{}", vp->getActualLeft(), vp->getActualTop(), vp->getActualWidth(), vp->getActualHeight()).c_str());
        ImGui::Text(fmt::format("Window.pixel:      {},{}", window->getWidth(), window->getHeight()).c_str());

        global->forEachVarPtr([](std::string name, float *vPtr, Global::VarScope<float> *scope)
                              { 
                            float min  = scope?scope->min:0;
                            float max = scope?scope->max:100;
                            ImGui::SliderFloat(name.c_str(), vPtr, min, max); });
        // stats

        const Ogre::RenderTarget::FrameStats &fs = window->getStatistics();
        ImGui::Text(fmt::format("FPS:     {:.2f}", fs.lastFPS).c_str());
        ImGui::Text(fmt::format("Tris:    {}", fs.triangleCount).c_str());
        ImGui::Text(fmt::format("Batches: {}", fs.batchCount).c_str());

        // quit confirm popup
        char *confirmPopupId = "Confirm";
        if (ImGui::Button("Quit"))
        {
            ImGui::OpenPopup(confirmPopupId);
        }

        if (ImGui::BeginPopupModal(confirmPopupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Quit? Are you sure?");
            ImGui::Separator();

            if (ImGui::Button("Yes"))
            {
                // 处理“确定”逻辑
                core->getImGuiApp()->breakRender();
                ImGui::CloseCurrentPopup(); // 关闭弹窗
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                // 可选：处理“取消”逻辑
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
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