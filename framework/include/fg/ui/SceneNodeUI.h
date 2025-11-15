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
#include <fmt/format.h>
#include <random>
#include <OgreRenderWindow.h>
#include "fg/demo/GameTerrain.h"
#include "ActiveTrayUI.h"

class SceneNodeUI
{
    template <typename... Args>
    using NodeVistFunc = void (*)(int depth, Node *cNode, Args... args);

    template <typename... Args>
    using SceneNodeVistFunc = void (*)(int depth, SceneNode *cNode, Args... args);

    Global *global;
    Core *core;
    bool breakRenderRequested = false;
    RenderWindow *window;
    Viewport *vp;
    SceneManager *sceMgr;
    ActiveTrayUI *activeTrayUI = nullptr;

public:
    SceneNodeUI(Core *core)
    {
        this->core = core;
        this->global = core->getGlobal();
        this->window = core->getWindow();
        this->vp = core->getViewport();
        this->sceMgr = core->getSceneManager();
        // active tray
    }

    void Open()
    {

        ImGui::Begin("SceneNode Explorer");

        SceneNode *sNode = sceMgr->getRootSceneNode();

        SceneNodeUI::forEachSceneNode<>(0, sNode, (SceneNodeVistFunc<>)[](int depth, SceneNode *cNode)
                                        {
            ImGui::Indent(depth * 10.0f);
            ImGui::Text(fmt::format("SceneNode: {}", cNode->getName()).c_str());
            ImGui::Unindent(depth * 10.0f); });

        ImGui::End();
    }
    template <typename... Args>
    static void forEachSceneNode(int depth, SceneNode *cNode, SceneNodeVistFunc<> func)
    {

        forEachNode<SceneNodeVistFunc<>>( //
            depth, cNode,                 //
            (NodeVistFunc<SceneNodeVistFunc<>>)[](int depth, Node *cNode, SceneNodeVistFunc<> func)
            {
                SceneNode* sceCNode = dynamic_cast<SceneNode*>(cNode);
                if(sceCNode){
                    func(depth, sceCNode);
               } },
            func);
    }

    template <typename... Args>
    static void forEachNode(int depth, Node *cNode, void (*func)(int, Node *, Args...), Args... args)
    {
        func(depth, cNode, args...);
        auto &children = cNode->getChildren();
        for (auto &child : children)
        {
            forEachNode(depth + 1, child, func, args...);
        }
    }
};