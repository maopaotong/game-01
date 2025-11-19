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
#include "fg/Property.h"
namespace fog
{
    class PropertyRefsUI : public UIState
    {
    protected:
        Core *core;

    public:
        PropertyRefsUI(UIState *pState, Core *core) : UIState(pState)
        {
            this->core = core;
        }

        bool open() override
        {
            if (!ImGui::Begin("PropertyRefs"))
            {
                return false;
            }

            Context<Property::Bag *>::get()->forEach([](const std::string &name, const Options::Option *option)
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

            ImGui::End();
            return true;
        }

        void onApply()
        {
            //
             core->getGame()->apply(nullptr);
        }
    };

}; // end of namespace