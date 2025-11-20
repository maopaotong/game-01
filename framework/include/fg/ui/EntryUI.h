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
#include "PropertyRefsUI.h"
#include "StatisticUI.h"
#include "QuitUI.h"
#include "TasksUI.h"
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
        EntryUI(Core *core, CostMap *costMap) : UIState("EntryUI"), core(core), costMap(costMap)
        {
            this->active = true;
        }

        void init() override
        {

            this->add(new OptionsUI(core));
            this->add(new PropertyRefsUI(core));
            this->add(new ActiveTrayUI(core, costMap));
            this->add(new StatisticTrayUI(core, costMap));
            this->add(new TasksUI(core));

            //Quit:
            this->add(new QuitUI(core, costMap));
        }
        void add(UIState *child)
        {
            child->init();
            UIState::add(child);
            ChildInfo ci = {child, child->getName()};
            childInfos.push_back(ci);
        }
        bool open() override
        {
            bool ret = UIState::open();
            if (!ret)
            {
                if (ImGui::Begin("Boot"))
                {
                    if (ImGui::Button("EntryUI"))
                    {
                        this->active = true;
                    }
                }
                ImGui::End();
            }

            return true;
        }
        void doOpen() override
        {

            for (auto ci : childInfos)
            {
                // if (ImGui::Button(ci.name.c_str()))
                if (ImGuiUtil::MyToggleButton(ci.name.c_str(), ci.ui->activePtr()))
                {
                    ci.ui->changeActive();
                }
            }
        };
    };

}; // end of namespace