#pragma once
#include "imgui.h"
#include <string>
#include "fg/Core.h"
#include "fg/State.h"
#include "fg/Event.h"

namespace fog{
class ActiveTrayUI : public Listener<Actor *, std::string &>
{
    Core *core;
    State *state = nullptr;

public:
    ActiveTrayUI(Core *core)
    {
        this->core = core;

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

        }
        else
        {
            ImGui::Text("No Active State");
        }

        ImGui::End();
        return true;
    }
};

};//end of namespace