#pragma once
#include "imgui.h"
#include <string>
#include "fg/Core.h"
#include "fg/State.h"
#include "fg/Ground.h"
#include "fmt/format.h"
#include "fg/util/ImGuiUtil.h"
#include "UIState.h"
#include "fg/Options.h"
#include "fg/Master.h"
namespace fog
{
    class TasksUI : public UIState
    {
    protected:
        Core *core;
        Options options;

    public:
        TasksUI(Core *core) : UIState("TasksUI")
        {
            this->core = core;
        }

        void doOpen() override
        {
            int id = 0;
            Context<Master *>::get()->forEachTask([](long id, Task *task)
                                                  {
                                                      ImGui::Text(("id:" + std::to_string(id)).c_str()); //
                                                      ImGui::SameLine();                                 //
                                                      ImGui::Text(("type:" + std::string(task->getTaskType().name())).c_str());
                                                      //
                                                  });

            ImGui::SameLine();

            if (ImGui::Button("Return"))
            {
                this->active = false;
            }
        }

        void onApply()
        {
            //
            core->getGame()->apply(&this->options);
        }
    };

}; // end of namespace