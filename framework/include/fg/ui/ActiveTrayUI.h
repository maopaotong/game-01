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
    class ActiveTrayUI : public Listener<State *, std::string &>, public UIState
    {
        Core *core;
        State *state = nullptr;
        CostMap *costMap;
        Property::Ref<Vector3> actorPosition;

    public:
        ActiveTrayUI(Core *core, CostMap *costMap) : UIState("ActiveActor")
        {
            this->core = core;
            this->costMap = costMap;
            // Context<ActorPropEC *>::get()->addListener(this);

            Context<Event::Bus *>::get()->subscribe<State *, std::string &>([this](State *s, std::string &ss)
                                                                            { this->onEvent(s, ss); });
        }
        void init() override
        {
            actorPosition = this->getProperty<Vector3>("actor1"".actor.position", false);
            UIState::init();
        }

        bool onEvent(State *a, std::string &pName) override
        {
            if (pName == "active")
            {
                State *s = dynamic_cast<State *>(a);

                if (s && s->isActive())
                {
                    this->state = s;
                }
                else
                {
                    this->state = nullptr;
                }
                // do something with s
            }
            return true;
        }

        void doOpen() override
        {

            if (state)
            {
                ImGui::Text(("Active State: " + std::to_string((uintptr_t)state)).c_str());
                                
                //CellKey ck = state->getDestinationCell();
                //Vector2 pos2D = CellUtil::calculateCenter(ck.first, ck.second, costMap);
                // Vector3 pos3D = Ground::Transfer::to3D(pos2D, Global::getTerrainHeightAtPositionWithOffset);

                ImGuiUtil::Text(actorPosition);
            }
            else
            {
                ImGui::Text("No Active State");
                if (ImGui::Button("Active actor"))
                {
                }
            }
        }
    };

}; // end of namespace