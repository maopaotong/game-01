#pragma once
#include "fg/defines.h"

#include <typeindex>

#include <memory>
#include <type_traits>
#include <utility>
#include <Ogre.h>
#include <fg/Cell.h>
#include <fg/Context.h>
#include "OgreSceneNode.h"
#include "fg/util/CostMap.h"
#include "fg/PathFollow2.h"
#include "fg/core/PathState.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/Tasks.h"
#include "fg/Cell.h"
#include "fg/CellInstanceManager.h"

namespace fog
{
    using BaseTask = Tasks::Task;
    /**
     * PathingStateManager find a path from current state position to mouse position.
     */
    class PathingStateManager : public State, public Stairs
    {
        State *currentPicked;
        float actorHighOffset;
        CellInstanceState *targetCis;
        PathState *currentPath;

    protected:
        CellKey cKey2;
        CellKey cKey1;

    public:
        PathingStateManager() : currentPicked(nullptr), targetCis(nullptr), currentPath(nullptr), actorHighOffset(.0f)
        {
            std::cout << "PathingStateManager created." << std::endl;
            Context<Event::Bus *>::get()-> //
                subscribe<EventType, State *>([this](EventType evtType, State *state)
                                              {
                                                  if (evtType == EventType::MovableStateUnpicked)
                                                  {
                                                      this->unPick(state);
                                                  }
                                                  else if (evtType == EventType::MovableStatePicked)
                                                  {
                                                      this->pick(state);
                                                  }
                                                  return true; //
                                              });
        }
        virtual ~PathingStateManager()
        {
            std::cout << "~PathingStateManager destroyed." << std::endl;
        }
        void unPick(State *state)
        {
            clearPath();
        }
        void clearPath()
        {
            if (this->currentPath)
            {
                this->removeChild(currentPath);
                this->currentPath = nullptr;
            }
        }
        void pick(State *state)
        {
            this->currentPicked = state;
        }

        CONSUMED onMouseMoved(int x, int y)
        {
            if (!this->currentPicked)
            {
                return false;
            }

            Viewport *viewport = Context<CoreMod *>::get()->getViewport();
            Camera *camera = Context<CoreMod *>::get()->getCamera();

            float ndcX = x / (float)viewport->getActualWidth();
            float ndcY = y / (float)viewport->getActualHeight();

            Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

            Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0); // Y = 0

            auto hitGrd = ray.intersects(ground);

            Ogre::Vector3 pos2;

            if (!hitGrd.first)
            {
                return false;
            }

            pos2 = ray.getPoint(hitGrd.second);
            CellInstanceManager *cellInstMgrState = Context<CellInstanceManager *>::get();
            CellInstanceState *cis = cellInstMgrState->getCellInstanceStateByPosition(pos2);
            if (!cis)
            {
                return false;
            }
            if (cis == targetCis)
            {
                return false;
            }
            if (targetCis)
            {
                targetCis->unsetColour();
            }
            cis->setColour(ColourValue::White);
            targetCis = cis;

            return false;
        }

        GOON step(float time) override
        {
            if (!currentPicked)
            {
                return true;
            }
            if (!targetCis)
            {
                return true;
            }

            CellInstanceState *sourceCis = nullptr;
            // find source cell
            MovableStateManager *movableStateMgr = Context<MovableStateManager *>::get();
            movableStateMgr->forEach([&sourceCis](State *state)
                                     {
                                         if (state->isActive())
                                         {
                                             sourceCis = Context<CellInstanceManager *>::get()->getCellInstanceStateByPosition(state->getSceneNode()->getPosition());
                                             if (sourceCis)
                                             {
                                                 return false; // break
                                             }
                                         } //
                                         return true; });
            if (!sourceCis)
            {
                return true;
            }
            //
            CellKey cKey2 = targetCis->getCellKey();
            CellKey cKey1 = sourceCis->getCellKey();
            if (this->cKey2 == cKey2 && this->cKey1 == cKey1)
            {                // do nothing.
                return true; // GOON
            }
            // update path
            this->cKey1 = cKey1;
            this->cKey2 = cKey2;

            std::vector<Vector2> pathByPoint2DNom = Context<CostMap *>::get()->findPath(cKey1, cKey2);

            PathState *pathState2 = new PathState();
            pathState2->init();
            pathState2->setPath(pathByPoint2DNom, cKey1, cKey2);
            this->clearPath();
            currentPath = pathState2;
            this->addChild(currentPath);
            return true;
        }

    }; //
};