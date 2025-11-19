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

namespace fog
{
    class QuitUI : public UIState
    {
        Global *global;
        Core *core;
        bool breakRenderRequested = false;
        RenderWindow *window;
        Viewport *vp;
        SceneManager *sceMgr;
        ActiveTrayUI *activeTrayUI = nullptr;

    public:
        QuitUI(Core *core, CostMap *costMap) : UIState("Quit")
        {
            this->core = core;
            this->global = core->getGlobal();
            this->window = core->getWindow();
            this->vp = core->getViewport();
            this->sceMgr = core->getSceneManager();

            //
        }

        void doOpen()
        {

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
        }
    };
}; // end of namespace