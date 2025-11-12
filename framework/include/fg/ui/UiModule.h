#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include "fg/Module.h"

class Example
{
public:
    class ImGuiModule : public Module
    {

    public:
        ImGuiModule()
        {
        }
        std::string getName() override
        {
            return "ui.imGui";
        }

        void active(Core *core) override
        {
            ImGui::CreateContext();
        }
    };

};
