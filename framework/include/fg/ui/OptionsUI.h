#pragma once
#include "imgui.h"
#include <string>
#include "fg/Core.h"
#include "fg/State.h"
#include "fg/Ground.h"
#include "fmt/format.h"
#include "fg/util/ImGuiUtil.h"
#include "UIState.h"

namespace fog
{
    template <typename T>
    struct Option
    {
        std::string name;
        T value;
        Option(std::string name, T value) : name(name), value(value) {}
    };

    class OptionsUI : public UIState
    {
        std::vector<Option<bool>*> boolOptions;

    public:
        OptionsUI(UIState *pState) : UIState(pState)
        {
            this->boolOptions.push_back(new Option<bool>("Draw All Cell", false));
        }

        bool open() override
        {
            if (!ImGui::Begin("Options"))
            {
                return false;
            }

            for (auto op : this->boolOptions)
            {

                if (ImGui::Checkbox(op->name.c_str(), &op->value))
                {
                    // ignore bool change.
                }
            }

            if (ImGui::Button("Apply"))
            {
                this->active = false;
            }

            ImGui::End();
            return true;
        }
    };

}; // end of namespace