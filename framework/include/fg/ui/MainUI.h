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
#include "ActiveTrayUI.h"

namespace fog{
class MainUI
{
    Global *global;
    Core *core;
    bool breakRenderRequested = false;
    RenderWindow *window;
    Viewport *vp;
    SceneManager *sceMgr;
    ActiveTrayUI *activeTrayUI = nullptr;

public:
    MainUI(Core *core,CostMap* costMap)
    {
        this->core = core;
        this->global = core->getGlobal();
        this->window = core->getWindow();
        this->vp = core->getViewport();
        this->sceMgr = core->getSceneManager();
        this->initGlobalVarPtr(core->getGlobal());
        // active tray

        //
    }
    static void forEachVarPtr(const std::string name, Actor **vPtr, Var<Actor*>::Range *range, int &actors)
    {
        Actor *a = *vPtr;
        if (a)
        {
            actors++;
            ImGui::Text(fmt::format("Actor:{}", name).c_str());
        }
    };
    void Open()
    {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));

        ImGui::Begin("Hello");

        ImGui::Text(fmt::format("Actors in global var bag:").c_str());
        ImGui::Indent(15.0f);
        int actors = 0;

        this->global->Var<Actor*>::Bag::forEachVarPtr<int &>(MainUI::forEachVarPtr, actors);
        ImGui::Unindent(15.0f);

        vp = core->getViewport();

        ImGui::Text(fmt::format("Viewport.norm:     {},{},{},{}", vp->getLeft(), vp->getTop(), vp->getWidth(), vp->getHeight()).c_str());
        ImGui::Text(fmt::format("Viewport.pixel:    {},{},{},{}", vp->getActualLeft(), vp->getActualTop(), vp->getActualWidth(), vp->getActualHeight()).c_str());
        ImGui::Text(fmt::format("Window.pixel:      {},{}", window->getWidth(), window->getHeight()).c_str());

        int counter = 0;
        this->global->Var<float>::Bag::forEachVarPtr<int &>(MainUI::forEachVarPtr, counter);

        this->global->Var<Vector3>::Bag::forEachVarPtr<int &>(MainUI::forEachVarPtr, counter);

        Vector3 pO = Global::Context<Node2D*>::get()->plane->getOrigin();

        ImGui::Text(fmt::format("({:.1f},{:.1f},{:.1f})", pO.x, pO.y, pO.z).c_str());
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
    static void forEachVarPtr(const std::string name, Vector3 *vPtr, int &counter)
    {
        ImGui::Text(fmt::format("{}: {:.2f}, {:.2f}, {:.2f}", name, (*vPtr)[0], (*vPtr)[1], (*vPtr)[2]).c_str());
    }
    static void forEachVarPtr(const std::string name, float *vPtr, Var<float>::Range *range, int &counter)
    {
        float min = range ? range->min : 0;
        float max = range ? range->max : 100;
        ImGui::SliderFloat(name.c_str(), vPtr, min, max);
    }

    void initGlobalVarPtr(Global *glb)
    {
        glb->Var<float>::Bag::createBindVptr(".aniSpeed", 0.55f, 0.0f, 2.0f);
        glb->Var<float>::Bag::createBindVptr(".pathSpeed", 1.0f, 1.0f, 10.0f);
    }
};
};//end of namespace