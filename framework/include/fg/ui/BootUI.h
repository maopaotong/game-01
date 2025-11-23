#pragma once
#include "imgui.h"
#include <string>
#include "fg/CoreMod.h"
#include "fg/State.h"
#include "fg/Ground.h"
#include "fmt/format.h"
#include "fg/util/ImGuiUtil.h"
#include "UIState.h"
#include "OptionsUI.h"
#include "PropertyRefsUI.h"
#include "StatisticUI.h"
namespace fog
{

    class BootUI : public UIState
    {
        
    public:
        BootUI(CoreMod *core, CostMap *costMap) : UIState("Boot")
        {
            this->active = true;
        }

        void init() override
        {

        }
        
        void doOpen() override
        {

        };
    };

}; // end of namespace