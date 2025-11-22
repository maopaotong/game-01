#pragma once
#include <string>
#include <vector>
#include <typeindex>

#include <vector>
#include <memory>
#include <type_traits>
#include <functional>
#include <utility>
#include <Ogre.h>
#include <fg/Cell.h>
#include <fg/Context.h>
#include "OgreSceneNode.h"
#include "fg/util/CostMap.h"
#include <fg/Global.h>
#include "fg/PathFollow2.h"
#include "fg/core/PathState.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/Tasks.h"
#include "fg/Cell.h"
#include "fg/Targets.h"
#include "fg/MeshBuild.h"
#include "fg/demo/ActorSelectControl.h"

namespace fog
{
    using BaseTask = Tasks::Task;
    using BaseOwner = Tasks::Owner;
    class KeepWorld
    {
    public:
        class Owner : public BaseOwner, public BaseTask
        {

        public:
            State *state;

            CostMap *costMap;
            Core *core;

        public:
            Owner(CostMap *costMap, Core *core, State *state) : costMap(costMap), core(core), state(state), BaseOwner(1), BaseTask(nullptr, nullptr)
            {
                this->doPush(this);
            }

            bool step(float time) override
            {
                return true;
            }
            bool pause() override
            {
                return true;
            }

            bool resume() override
            {
                return true;
            }
            bool wait(Task *toWait) override
            {
                return true;
            }

            void destroy() override
            {
                // todo destoy the owner by remove owner from monitor.
            }
        };

        class Task : public BaseTask
        {

            Owner *owner;
            Targets::KeepWorld *target;

        protected:
        public:
            Task(Targets::KeepWorld *target, Owner *owner, State *state) : target(target), owner(owner), //
                                                                           Tasks::Task(target, owner)
            {
            }

            void rebuildMesh()
            {
            }

            void destroy() override
            {
            }

            bool pause() override
            {
                return false;
            }

            bool resume() override
            {
                return false;
            }

            bool wait(Tasks::Task *toWait) override
            {
                return false;
            }

            bool step(float time) override
            {
                // todo improve performance by caching.

                this->owner->state->forEachChild([this](State *state) //
                                                 {
                                                     if (state->isActive())
                                                     {
                                                         State *tracker = state->getChildByName("tracker");
                                                         if (!tracker)
                                                         {
                                                             Targets::TrackActorByCell *target = new Targets::TrackActorByCell(state);
                                                             ActorSelectControl *tracker = new ActorSelectControl(this->owner->costMap, this->owner->core);
                                                             tracker->init();
                                                             tracker->setName("tracker");
                                                             state->addChild(tracker);
                                                             if (!tracker->getTaskOwner()->tryTakeTarget(target))
                                                             {
                                                                 throw std::runtime_error("bug?");
                                                             }
                                                         }
                                                     }
                                                     return true;
                                                     //
                                                 });
                return true;
            }
        };

    }; //
};