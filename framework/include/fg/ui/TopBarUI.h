#pragma once
#include "imgui.h"
#include <string>
#include "fg/CoreMod.h"
#include "fg/State.h"
#include "fg/Event.h"
#include <fmt/format.h>
#include "UIState.h"
#include "fg/InventoryStateManager.h"
namespace fog
{
    class TopBarUI : /* public Listener<State *, std::string &>,*/ public UIState
    {
    public:
        TopBarUI() : UIState("TopBar")
        {
        }
        void init() override
        {
            // actorPosition = this->getProperty<Vector3>("actor1.position", false);
            UIState::init();
        }

        void doOpen() override
        {
            ImGui::Text("Population: %d", Context<InventoryStateManager>::get()->getTotalPopulation());
        }
    };

}; // end of namespace