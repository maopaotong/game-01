// main.cpp - Complete Ogre A* Hex Grid Visualization System
#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <algorithm>
#include <functional>

// === Include OgreBites for modern initialization ===
#include <Bites/OgreApplicationContext.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreFrameListener.h>
#include <OgreRTShaderSystem.h>
#include <OgreTechnique.h>
#include <OgreRenderWindow.h>
#include <iostream>
#include "fg/util/CellMark.h"
#include "fg/util/CellUtil.h"
#include "fg/Pickable.h"
namespace fog
{
    using namespace OgreBites;
    using namespace Ogre;
    // === Custom hash function ===
    //
    // === Input handler for closing application ===
    class MouseClickPicker : public OgreBites::InputListener
    {
    private:
        Viewport *viewport;
        Camera *camera;
        SceneManager *sMgr;
        State *state;

    public:
        MouseClickPicker(State *state, Camera *cam, SceneManager *sMgr, Viewport *vp) : state(state)
        {
            this->camera = cam;
            this->sMgr = sMgr;
            this->viewport = vp;
        }

        bool mousePressed(const MouseButtonEvent &evt) override
        {
            if (evt.button == ButtonType::BUTTON_LEFT)
            {
                return mouseButtonLeftPressed(evt);
            }
            else if (evt.button == ButtonType::BUTTON_RIGHT)
            {
                return mouseButtonRightPressed(evt.x, evt.y);
            }
            return true;
        }
        bool mouseButtonLeftPressed(const MouseButtonEvent &evt)
        {
            // normalized (0,1)

            float ndcX = evt.x / (float)viewport->getActualWidth();
            float ndcY = evt.y / (float)viewport->getActualHeight();
            Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);
            State *picked = this->pick(ray);

            if (picked)
            {
                picked->setActive(true);
                return false;
            }
            else
            {
                this->state->forEach([](State *state)
                                     {
                                         if (state->isActive())
                                         {
                                             state->setActive(false);
                                         } //
                                         return true; //
                                     });
            }
            return true;
        }

        bool mouseButtonRightPressed(int mx, int my)
        {

            // normalized (0,1)
            Viewport *viewport = Context<CoreMod *>::get()->getViewport();
            Camera *camera = Context<CoreMod *>::get()->getCamera();

            float ndcX = mx / (float)viewport->getActualWidth();
            float ndcY = my / (float)viewport->getActualHeight();

            Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

            Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0); // Y = 0
            auto hitGrd = ray.intersects(ground);
            std::cout << "ndc:(" << ndcX << "," << ndcY << ")" << "hit:" << hitGrd.first << std::endl;
            if (hitGrd.first)
            {
                Ogre::Vector3 pos = ray.getPoint(hitGrd.second);

                // bool hitCell = CellUtil::findCellByPoint(costMap, Vector2(pos.x, pos.z), cKey);
                // bool hitCell = CellUtil::findCellByPoint(costMap, Ground::Transfer::to2D(pos), cKey);
                Cell::Instance cell;
                bool hitCell = Context<Cell::Center *>::get()->findCellByWorldPosition(pos, cell);
                if (hitCell)
                {
                    CellKey cKey = cell.cKey;

                    // find all active state
                    // Move to cell task.
                    Context<State *>::get()->forEach([this, &cKey](State *state)
                                                     {
                                                         if (state->isActive())
                                                         {
                                                             MoveToCellTask *task = new MoveToCellTask(state, cKey);
                                                             state->getTaskRunner()->pushOrWait(task);
                                                         }
                                                         return true; //
                                                     });
                    //
                }
                // cout << "worldPoint(" << pickX << ",0," << pickZ << "),cellIdx:[" << cx << "," << cy << "]" << endl;
            }
            return true;
        }

        State *pick(Ray &ray)
        {
            // 创建射线查询对象
            Ogre::RaySceneQuery *rayQuery = sMgr->createRayQuery(ray);
            rayQuery->setSortByDistance(true);  // 按距离排序（最近的优先）
            rayQuery->setQueryMask(0x00000001); // 与 Entity 的查询掩码匹配

            // 执行查询
            Ogre::RaySceneQueryResult &result = rayQuery->execute();

            State *state = nullptr;
            // 遍历结果
            for (auto &it : result)
            {
                Node *node = it.movable->getParentNode();
                State *s = State::get(node);
                if (s && s->pickable())
                {
                    state = s;
                    break;
                }
            }

            sMgr->destroyQuery(rayQuery);
            return state;
            // high light the cell in which the actor stand.
        }
    };

}; // end of namespace