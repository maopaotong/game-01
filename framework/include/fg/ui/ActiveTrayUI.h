#pragma once
#include "imgui.h"
#include <string>
#include "fg/Core.h"
#include "fg/State.h"
#include "fg/EventCenter.h"
#include "fg/PropertyEvent.h"
class ActiveTrayUI
{
    Core *core;
    State *state = nullptr;

public:
    ActiveTrayUI(Core *core)
    {
        this->core = core;

        std::function<bool(PropertyEvent<State> &)> func = [&](PropertyEvent<State> &evt)
        {
            if (evt.getName() == "active")
            {
                State *s = evt.getState();
                if (s->isActive())
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
        };

        this->core->getEventCenter()->addListener<PropertyEvent<State>>(func);
    }

    bool Open()
    {
        if (!ImGui::Begin("Active Tray"))
        {
            return false;
        }

        State *state = this->state;
        if (state)
        {
            ImGui::Text(("Active State: " + std::to_string((uintptr_t)state)).c_str());
        }
        else
        {
            ImGui::Text("No Active State");
        }

        ImGui::End();
        return true;
    }
};
