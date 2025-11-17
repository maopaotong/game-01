#pragma once
#include "imgui.h"
#include <string>
#include "fg/Core.h"
#include "fg/State.h"
#include "fg/Event.h"
#include <fmt/format.h>
namespace fog
{
    class ActiveTrayUI : public Listener<Actor *, std::string &>
    {
        Core *core;
        State *state = nullptr;
        CostMap *costMap;

    public:
        ActiveTrayUI(Core *core, CostMap *costMap)
        {
            this->core = core;
            this->costMap = costMap;
            Global::Context<ActorPropEC *>::get()->addListener(this);
        }

        bool onEvent(Actor *a, std::string &pName) override
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

        bool Open()
        {
            if (!ImGui::Begin("Active Tray"))
            {
                return false;
            }

            if (state)
            {
                ImGui::Text(("Active State: " + std::to_string((uintptr_t)state)).c_str());
                CellKey ck = state->getDestinationCell();

                Vector2 pos2D = CellUtil::calculateCenter(ck.first, ck.second, costMap);
                Vector3 pos3D = Ground::Transfer::to3D(pos2D, Global::getTerrainHeightAtPositionWithOffset);
                ImGui::Text(fmt::format("DistinationCell:[{},{}]({},{},{})", ck.first, ck.second, pos3D.x, pos3D.y, pos3D.z).c_str());
            }
            else
            {
                ImGui::Text("No Active State");
                if (ImGui::Button("Active actor"))
                {
                    
                }
            }

            ImGui::End();
            return true;
        }
    };

}; // end of namespace