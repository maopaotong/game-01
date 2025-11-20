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
    class PolicyUI : public UIState
    {
        Core *core;
        State *state = nullptr;
        CostMap *costMap;
        Property::Ref<Vector3> actorPosition;

    public:
        PolicyUI(Core *core, CostMap *costMap) : UIState("ActiveActor")
        {
            this->core = core;
            this->costMap = costMap;            
        }
        void init() override
        {            
            UIState::init();
        }

        void doOpen() override
        {

            if (state)
            {
                
            }
            else
            {
                
            }
        }
    };

}; // end of namespace