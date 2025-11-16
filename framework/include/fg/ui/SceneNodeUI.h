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

namespace fog{
class SceneNodeUI
{
    template <typename... Args>
    using NodeVistFunc = bool (*)(int depth, Node *cNode, Args... args);

    template <typename... Args>
    using SceneNodeVistFunc = bool (*)(int depth, SceneNode *cNode, Args... args);

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
        int id = 0;
        SceneNodeVistFunc<int &> func = (SceneNodeVistFunc<int &>)[](int depth, SceneNode *cNode, int &id) -> bool
        {
            float indent = (depth + 1) * 10.0f;
            ImGui::Indent(indent);
            ImGui::Text(fmt::format("SceneNode: {}", cNode->getName()).c_str());
            Ogre::Any expand = cNode->getUserObjectBindings().getUserAny(".expanding");            
            bool isExpand = expand.isEmpty()?false: expand.get<bool>();
            
            ImGui::PushID(id++);
            if(cNode->getChildren().size()>0){
                ImGui::SameLine();
                if(ImGui::Checkbox("Exp:", &isExpand)){
                    cNode->getUserObjectBindings().setUserAny(".expanding", Ogre::Any(isExpand));
                }
            }

            ImGui::PopID();

            ImGui::Unindent(indent); 
            return isExpand; };
        SceneNodeUI::forEachSceneNode<int &>(0, sNode, func, id);

        ImGui::End();
    }
    template <typename... Args>
    static void forEachSceneNode(int depth, SceneNode *cNode, SceneNodeVistFunc<Args...> func, Args... args)
    {

        forEachNode<SceneNodeVistFunc<Args...>, Args...>( //
            depth, cNode,                                 //
            (NodeVistFunc<SceneNodeVistFunc<Args...>, Args...>)[](int depth, Node *cNode, SceneNodeVistFunc<Args...> func, Args... args) -> bool
            {
                SceneNode *sceCNode = dynamic_cast<SceneNode *>(cNode);
                if (sceCNode)
                {
                    return func(depth, sceCNode, args...);
                }
                return true;
            },
            func, args...);
    }

    template <typename... Args>
    static void forEachNode(int depth, Node *cNode, bool (*func)(int, Node *, Args...), Args... args)
    {
        bool goOn = func(depth, cNode, args...);
        if (!goOn)
        {
            return;
        }
        auto &children = cNode->getChildren();
        for (auto &child : children)
        {
            forEachNode<Args...>(depth + 1, child, func, args...);
        }
    }
};
};//end of namespace