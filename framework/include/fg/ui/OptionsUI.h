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
        OptionsUI(UIState *pState, Core *core) : UIState(pState)
        {
            this->core = core;
            options.add<bool>("Show-plain-cell?", false);
            options.add<std::string>("Video-Mode", "1024 x 768");
        }

        bool open() override
        {
            if (!ImGui::Begin("Options"))
            {
                return false;
            }

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
            if (ImGui::Button("Start"))
            {
                this->active = false;
                this->onStart();
            }

            ImGui::SameLine();

            if (ImGui::Button("Return"))
            {
                this->active = false;
            }

            ImGui::End();
            return true;
        }

        void onStart()
        {
            //
            core->getGame()->start(&this->options);
        }
    };

}; // end of namespace