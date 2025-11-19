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
    class OptionsUI : public UIState
    {
    protected:
        Core *core;
        Options options;

    public:
        OptionsUI(UIState *pState, Core *core) : UIState(pState, "Options")
        {
            this->core = core;
            options.add("Show-plain-cell?", false);
            options.add("Video-Mode", "1024 x 768");
        }

        void doOpen() override
        {

            this->options.forEach([](const std::string name, const Options::Option *option)
                                  {
                                      if (option->isType<bool>())
                                      {
                                          bool &valuePtr = option->getValueRef<bool>();

                                              if (ImGui::Checkbox(option->name.c_str(), &valuePtr))
                                              {
                                                  // ignore bool change.
                                              }
                                          

                                      }else if(option->isType<std::string>()){

                                        std::string &valuePtr = option->getValueRef<std::string>();

                                        ImGui::Text((valuePtr).c_str());
                                      
                                        } });

            //
            if (ImGui::Button("Apply"))
            {
                this->onApply();
            }

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