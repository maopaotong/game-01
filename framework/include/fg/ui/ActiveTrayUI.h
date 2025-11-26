#pragma once
#include "imgui.h"
#include <string>
#include "fg/CoreMod.h"
#include "fg/State.h"
#include "fg/Event.h"
#include <fmt/format.h>
#include "UIState.h"
namespace fog
{
    class ActiveTrayUI : /* public Listener<State *, std::string &>,*/ public UIState
    {
        State *state;

    public:
        ActiveTrayUI() : UIState("ActiveActor"), state(nullptr)
        {
        }
        void init() override
        {
            // actorPosition = this->getProperty<Vector3>("actor1.position", false);
            UIState::init();
            Context<Event::Bus>::get()-> //
                subscribe<MovableEventType, State *>([this](MovableEventType et, State *s)
                                                     {
                    if (et == MovableEventType::StatePicked)
                    {
                        this->state = s;
                        this->setActive(true);
                    }
                    else if (et == MovableEventType::StateUnpicked)
                    {
                        this->state = nullptr;
                        this->setActive(false);
                    } });

            
        }

        void doOpen() override
        {
            int counter = 0;
            if (state)
            {

                SceneNode *sNode = state->findSceneNode();
                ImGui::Text(("Active State: " + std::to_string((uintptr_t)state)).c_str());
                ImGui::SameLine();
                ImGuiUtil::Text(sNode->getPosition());
                counter++;
                //
                if (ImGui::Button("Tower Building"))
                {
                    Context<BuildingStateManager>::get()->planToBuild();
                }
            }

            if (!counter)
            {
                ImGui::Text("No Active State");
                if (ImGui::Button("Active actor"))
                {
                }
            }
        }
    };

}; // end of namespace