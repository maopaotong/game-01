#pragma once
#include "imgui.h"
#include <string>
#include "fg/Core.h"
#include "fg/State.h"
#include "fg/EventCenter.h"
#include "fg/PropertyEvent.h"

class ActiveTrayUI : public Listener<PropertyEvent<Actor> &>
{
    Core *core;
    State *state = nullptr;

public:
    ActiveTrayUI(Core *core)
    {
        this->core = core;

        Global::Context<ECPEActor *>::get()->addListener(this);
    }

    bool onEvent(PropertyEvent<Actor> &evt) override
    {
        if (evt.getName() == "active")
        {
            Actor *a = evt.getState();
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
