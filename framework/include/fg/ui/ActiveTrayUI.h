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
        // State *state = nullptr;
        // Property::Ref<Vector3> actorPosition;

    public:
        ActiveTrayUI() : UIState("ActiveActor")
        {
            // Context<ActorPropEC *>::get()->addListener(this);

            // Context<Event::Bus *>::get()->subscribe<State *, std::string &>([this](State *s, std::string &ss)
            //                                                                 { this->onEvent(s, ss); });
        }
        void init() override
        {
            // actorPosition = this->getProperty<Vector3>("actor1.position", false);
            UIState::init();
        }

        // bool onEvent(State *a, std::string &pName) override
        // {
        //     if (pName == "active")
        //     {
        //         State *s = dynamic_cast<State *>(a);

        //         if (s && s->isActive())
        //         {
        //             this->state = s;
        //         }
        //         else
        //         {
        //             this->state = nullptr;
        //         }
        //         // do something with s
        //     }
        //     return true;
        // }

        void doOpen() override
        {
            int counter = 0;
            Context<State *>::get()->forEach([&counter](State *state)
                                             {
                                                 if (state->isActive())
                                                 {
                                                     SceneNode *sNode = state->findSceneNode();
                                                     if (sNode)
                                                     {

                                                         ImGui::Text(("Active State: " + std::to_string((uintptr_t)state)).c_str());
                                                         ImGui::SameLine();
                                                         ImGuiUtil::Text(sNode->getPosition());
                                                         counter++;
                                                     }
                                                 }
                                                 return true; //
                                             });

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