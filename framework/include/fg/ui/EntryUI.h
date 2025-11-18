#pragma once
#include "imgui.h"
#include <string>
#include "fg/Core.h"
#include "fg/State.h"
#include "fg/Ground.h"
#include "fmt/format.h"
#include "fg/util/ImGuiUtil.h"
#include "UIState.h"
#include "OptionsUI.h"
#include "StatisticUI.h"
namespace fog
{

    class EntryUI : public UIState
    {
        struct ChildInfo
        {
            UIState *ui;
            std::string name;
        };

    protected:
        std::vector<ChildInfo> childInfos;
        Core *core;
        CostMap *costMap;

    public:
        EntryUI(UIState *pState, Core *core, CostMap *costMap) : UIState(pState), core(core), costMap(costMap)
        {
        }

        void init() override
        {

            this->add("Options", new OptionsUI(this, core));
            this->add("Active Actor", new ActiveTrayUI(this, core,costMap));
            this->add("Statistic", new StatisticTrayUI(this, core,costMap));
            
        }
        void add(std::string name, UIState *child)
        {
            UIState::add(child);
            ChildInfo ci = {child, name};
            childInfos.push_back(ci);
        }

        bool open() override
        {
            UIState::open();
            for (auto ci : childInfos)
            {
                if (ImGui::Button(ci.name.c_str()))
                {
                    ci.ui->changeActive();
                }
            }
            UIState::openChildren();
            return true;
        };
    };

}; // end of namespace