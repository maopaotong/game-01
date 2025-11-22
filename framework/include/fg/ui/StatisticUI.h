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
        State *state = nullptr;

    public:
        StatisticTrayUI() : UIState("StatisticTray")
        {
            // Context<ActorPropEC *>::get()->addListener(this);
        }

        void doOpen() override
        {

            RenderWindow *window = Context<Core*>::get()->getWindow();
            const Ogre::RenderTarget::FrameStats &fs = window->getStatistics();
            ImGui::Text(fmt::format("FPS:     {:.2f}", fs.lastFPS).c_str());
            ImGui::Text(fmt::format("Tris:    {}", fs.triangleCount).c_str());
            ImGui::Text(fmt::format("Batches: {}", fs.batchCount).c_str());
        }
    };

}; // end of namespace