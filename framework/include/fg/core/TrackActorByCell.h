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
#include "fg/MeshBuild.h"

namespace fog
{
    using BaseTask = Tasks::Task;
    class TrackActorByCellTask : public BaseTask
    {

    protected:
        Core *core;
        CostMap *costMap;
        State *actor;
        //

        CellKey cKey;
        Vector3 prePosition;
        ManualObject *obj;
        std::string material;
        State *state;

    public:
        TrackActorByCellTask(CostMap *costMap, Core *core, State *state, ManualObject *obj, std::string material) : state(state), material(material), obj(obj), core(core), costMap(costMap)
        {
        }

        void rebuildMesh()
        {

            MeshBuild::SpiderNet buildMesh(this->obj);
            buildMesh.begin(this->material);
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
            Vector3 actorPosition = this->state->getSceneNode()->getPosition();

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

    }; //
};