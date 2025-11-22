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
            
            core->getRootState()->forEachChild([](State* state)
            {
                Tasks::Runner * owner = state->getTaskRunner();

                if(owner){
                    ImGui::Text(("owner.stackSize:" + std::to_string(owner->stackSize())).c_str()); //
                    ImGui::SameLine();                                 //
                    ImGui::Text(("owner.queueSize:" + std::to_string(owner->queueSize())).c_str());
                    ImGui::SameLine();                                 //
                    ImGui::Text(("owner.popCounter:" + std::to_string(owner->getPopCounter())).c_str());
                    
                }
                return true;
                //
            });
            
            if (ImGui::Button("Return"))
            {
                this->active = false;
            }
        }

        void onApply()
        {
            //
           // core->getGame()->apply(&this->options);
        }
    };

}; // end of namespace