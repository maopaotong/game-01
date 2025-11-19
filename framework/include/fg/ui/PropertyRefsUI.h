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
        PropertyRefsUI(Core *core) : UIState("PropertyRefs")
        {
            this->core = core;
        }

        void doOpen() override
        {

            Context<Property::Bag *>::get()->forEach([](const std::string &name, const Options::Option *option)
                                                     {
                                                         ImGui::Text(option->name.c_str());
                                                         ImGui::SameLine();
                                                         if (option->isType<bool>())
                                                         {

                                                             bool &valuePtr = option->getValueRef<bool>();

                                                             if (ImGui::Checkbox("", &valuePtr))
                                                             {
                                                                 // ignore bool change.
                                                             }
                                                         }
                                                         else if (option->isType<std::string>())
                                                         {

                                                             std::string &valuePtr = option->getValueRef<std::string>();

                                                             ImGui::Text((valuePtr).c_str());
                                                         }
                                                         else if (option->isType<Vector3>())
                                                         {

                                                             Vector3 &valuePtr = option->getValueRef<Vector3>();

                                                             ImGuiUtil::Text(valuePtr);
                                                         }
                                                         else
                                                         {
                                                             ImGui::Text("TODO");
                                                         }
                                                         //
                                                     });

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
            core->getGame()->apply(nullptr);
        }
    };

}; // end of namespace