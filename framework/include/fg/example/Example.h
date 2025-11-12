#pragma once
#include "fg/core/SimpleApp.h"
#include "fg/Module.h"
#include "WorldStateControl.h"
#include "ExampleGround.h"
#include "imgui.h"

class Example
{
public:
    class CostMapMod : public Module
    {

    public:
        CostMapMod()
        {
        }
        std::string getName() override
        {
            return "example.costMapMod";
        }

        void active(Core *core) override
        {
            CostMap *costMap = new CostMapControl(12, 10);
            core->setUserObject<CostMap>("costMap", costMap);
        }
    };

    class WorldStateMod : public Module
    {

    public:
        WorldStateMod()
        {
        }
        std::string getName() override
        {
            return "example.worldStateMod";
        }

        void active(Core *core) override
        {

            // Create materials before buding mesh?
            MaterialFactory::createMaterials(core->getMaterialManager());

            CostMap *costMap = core->getUserObject<CostMap>("costMap", true);
            Ground *ground = new ExampleGround(costMap);
            State *world = new WorldStateControl(costMap, ground, core);
            SceneNode *node = core->getSceneManager()->getRootSceneNode();
            world->setSceneNode(node);
        }
    };

    class UIMod : public Module, public ImGuiApp::FrameListener
    {
        Global *global;

    public:
        UIMod()
        {
        }
        std::string getName() override
        {
            return "example.uiMod";
        }

        void active(Core *core) override
        {
            this->global = core->getGlobal();
            core->getImGuiApp()->addFrameListener(this);
        }

        void onFrame(const FrameEvent &evt)
        {
            ImGui::Begin("Hello");
            Actor *actor = global->getActiveActor();
            if (actor)
            {
                ImGui::Button(":Right click a cell as the destination!");
            }
            else
            {
                ImGui::Button(":Left click to pick an actor!");
            }

            ImGui::End();
        }
    };
};
