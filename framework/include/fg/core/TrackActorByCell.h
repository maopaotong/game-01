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

namespace fog
{
    using BaseTask = Tasks::Task;
    using BaseOwner = Tasks::Owner;
    class TrackActorByCell
    {
    public:
        class Owner : public BaseOwner, public BaseTask
        {

        public:

            std::string material;
            ManualObject *obj;

        public:
            Owner(ManualObject *obj, std::string material) : obj(obj), material(material), BaseOwner(1), BaseTask(nullptr, nullptr)
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
            Targets::TrackActorByCell *target;

        protected:
            Core *core;
            CostMap *costMap;
            State *actor;
            //

            CellKey cKey;
            Vector3 prePosition;

        public:
            Task(Targets::TrackActorByCell *target, Owner *owner, CostMap *costMap, Core *core) : core(core), costMap(costMap), //
                                                                                                  target(target), owner(owner), //
                                                                                                  Tasks::Task(target, owner)
            {
            }

            void rebuildMesh()
            {

                MeshBuild::SpiderNet buildMesh(owner->obj);
                buildMesh.begin(this->owner->material);
                Cell::Instance cell = Context<Cell::Center *>::get()->getCell(cKey);
                buildMesh(cell, ColourValue::White); //
                buildMesh.end();
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
                Cell::Center *cc = Context<Cell::Center *>::get();
                Cell::Instance cell2;
                Vector3 actorPosition = this->target->state->getSceneNode()->getPosition();

                if (prePosition.distance(actorPosition) > CostMap::hexSize)
                {

                    if (cc->findCellByWorldPosition(actorPosition, cell2))
                    {

                        if (this->cKey != cell2.cKey)
                        {
                            this->cKey = cell2.cKey;
                            this->rebuildMesh(); // todo move the medh build to input event stage, not in frame event?
                        }
                    }
                    else
                    { // cannot found the actor cell,what to do?
                    }
                    prePosition = actorPosition;
                }

                return true;
            }
        };

    }; //
};