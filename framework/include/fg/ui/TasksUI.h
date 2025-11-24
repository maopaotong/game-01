#pragma once
#include "imgui.h"
#include <string>
#include "fg/CoreMod.h"
#include "fg/State.h"
#include "fmt/format.h"
#include "fg/util/ImGuiUtil.h"
#include "UIState.h"
#include "fg/Options.h"
namespace fog
{
    class TasksUI : public UIState
    {
    protected:
        Options options;

    public:
        TasksUI() : UIState("TasksUI")
        {
        }

        void doOpen() override
        {
            int id = 0;
            
            Context<State*>::get()->forEachChild([](State* state)
            {
                Tasks::Slot * owner = state->tryGetSlot(0);

                if(owner){
                    ImGui::Text(("owner.stackSize:" + std::to_string(owner->stackSize())).c_str()); //
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