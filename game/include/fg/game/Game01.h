#pragma once
#include "fg/Global.h"
#include "fg/Core.h"
#include "fg/Module.h"
#include "fg/util/CostMap.h"
#include "fg/Ground.h"
#include "fg/core/MaterialFactory.h"
#include "fg/core/CostMapGround.h"
#include "fg/demo/WorldStateControl.h"

#include "imgui.h"

class Game01 : public Module, public ImGuiApp::FrameListener
{
    Global *global;

public:
    Game01()
    {
    }
    std::string getName() override
    {
        return "example.costMapMod";
    }

    void active(Core *core) override
    {
        CostMap *costMap = new CostMap(12, 10);
        // Create materials before buding mesh?
        MaterialFactory::createMaterials(core->getMaterialManager());
        Ground *ground = new CostMapGround(costMap);
        State *world = new WorldStateControl(costMap, ground, core);
        SceneNode *node = core->getSceneManager()->getRootSceneNode();
        world->setSceneNode(node);
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