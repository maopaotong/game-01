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
#include "fg/PathFollow2.h"
#include "fg/core/PathState.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/Tasks.h"
#include "fg/Cell.h"
#include "fg/MeshBuild.h"

namespace fog
{
    using BaseTask = Tasks::Task;
    class TrackActorTask : public BaseTask, public CellStateBase
    {

    protected:
        //

        CellKey cKey;
        Vector3 prePosition;
        State *state;

    public:
        TrackActorTask(State *state) : state(state)
        {
        }

        void rebuildMesh() override
        {

            MeshBuild::SpiderNet buildMesh(this->obj);
            buildMesh.begin(this->material);
            Cell::Instance cell = Context<Cell::Center *>::get()->getCell(cKey);
            buildMesh(cell, ColourValue::White); //
            buildMesh.end();
        }

        bool pause() override
        {
            return false;
        }

        bool resume() override
        {
            return false;
        }
        bool cancel() override
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
                    std::cout<<"Cannot found actor cell."<<std::endl;
                }
                prePosition = actorPosition;
            }

            return true;
        }

    }; //
};