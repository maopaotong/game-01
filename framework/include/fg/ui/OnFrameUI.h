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
#include "MainUI.h"
#include "SceneNodeUI.h"
namespace fog{
class OnFrameUI : public ImGuiApp::FrameListener
{
    Global *global;
    Core *core;
    bool breakRenderRequested = false;
    RenderWindow *window;
    Viewport *vp;
    SceneManager *sceMgr;
    ActiveTrayUI *activeTrayUI = nullptr;
    MainUI *mainUI = nullptr;
    SceneNodeUI *sceneNodeUI = nullptr;
public:
    OnFrameUI(Core *core)
    {
        this->core = core;
        this->global = core->getGlobal();
        this->window = core->getWindow();
        this->vp = core->getViewport();
        this->sceMgr = core->getSceneManager();

        //
        this->activeTrayUI = new ActiveTrayUI(core);
        this->mainUI = new MainUI(core);
        this->sceneNodeUI = new SceneNodeUI(core);
    }

    void onFrame(const FrameEvent &evt)
    {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
        this->mainUI->Open();
        this->activeTrayUI->Open();
        this->sceneNodeUI->Open();
        ImGui::PopStyleColor();
    }
};
};//end of namespace