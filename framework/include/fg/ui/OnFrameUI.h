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
#include "CommandUI.h"
#include "MainUI.h"
#include "SceneNodeUI.h"
#include "EntryUI.h"

namespace fog
{
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
        CommandUI *cmdUI;
        EntryUI *entryUI;

    public:
        OnFrameUI(Core *core, CostMap *costMap)
        {
            this->core = core;
            this->global = core->getGlobal();
            this->window = core->getWindow();
            this->vp = core->getViewport();
            this->sceMgr = core->getSceneManager();

            //
            this->activeTrayUI = new ActiveTrayUI(core, costMap);
            this->mainUI = new MainUI(core, costMap);
            this->sceneNodeUI = new SceneNodeUI(core);
            this->cmdUI = new CommandUI(core, costMap);

            this->entryUI = new EntryUI(nullptr, core);
            this->entryUI->init();
        }

        void onFrame(const FrameEvent &evt)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
            // this->mainUI->Open();
            // this->activeTrayUI->Open();
            // this->sceneNodeUI->Open();
            // this->cmdUI->Open();

            this->entryUI->open();

            ImGui::PopStyleColor();
        }
    };
}; // end of namespace