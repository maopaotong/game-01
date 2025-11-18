#pragma once
#include "imgui.h"
#include <string>
#include "fg/Core.h"
#include "fg/State.h"
#include "fg/Event.h"
#include <fmt/format.h>
#include "UIState.h"
namespace fog
{
    class StatisticTrayUI : public UIState
    {
        Core *core;
        State *state = nullptr;
        CostMap *costMap;

    public:
        StatisticTrayUI(UIState *pState, Core *core, CostMap *costMap) : UIState(pState)
        {
            this->core = core;
            this->costMap = costMap;
            // Global::Context<ActorPropEC *>::get()->addListener(this);
        }

        bool open() override
        {
            if (!ImGui::Begin("Statistic"))
            {
                return false;
            }
            RenderWindow *window = core->getWindow();
            const Ogre::RenderTarget::FrameStats &fs = window->getStatistics();
            ImGui::Text(fmt::format("FPS:     {:.2f}", fs.lastFPS).c_str());
            ImGui::Text(fmt::format("Tris:    {}", fs.triangleCount).c_str());
            ImGui::Text(fmt::format("Batches: {}", fs.batchCount).c_str());

            ImGui::End();
            return true;
        }
    };

}; // end of namespace