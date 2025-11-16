#pragma once
#include "imgui.h"
#include <string>
#include "fg/Core.h"
#include "fg/State.h"
#include "fg/EventCenter.h"

namespace fog{
class CommandUI
{
    Core *core;

public:
    CommandUI(Core *core)
    {
        this->core = core;
    }

    bool Open()
    {
        if (!ImGui::Begin("Command Tray"))
        {
            return false;
        }
        bool *isCellPickable = false;
        if (ImGui::Checkbox("Enable Cell Pickable", isCellPickable))
        {
            // input handling code
        }


        ImGui::End();
        return true;
    }
};

};//end of namespace