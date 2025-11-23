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
namespace fog
{
    using BaseTask = Tasks::Task;
    class MoveToCellTask : public BaseTask
    {
        std::vector<std::string> aniNames = {"RunBase", "RunTop"};

        float actorHighOffset;

    protected:
        CellKey cKey2;
        State *state;
        //
        PathFollow2MissionState *mission = nullptr;
        bool failed = false;
        PathState *pathState;
        PathFollow2 *pathFollow2;

    public:
        MoveToCellTask(State *state, CellKey cKey2) : state(state), cKey2(cKey2), BaseTask()
        {
        }

        void destroy() override
        {
            this->deleteMission();
        }

        void deleteMission()
        {
            if (this->mission)
            {
                state->removeChild(this->mission);
                delete this->mission;
                this->mission = nullptr;
                delete this->pathFollow2;
                this->pathFollow2 = nullptr;
                delete this->pathState;
                this->pathState = nullptr;
            }
        }

        bool pause() override
        {
            return false;
        }

        bool resume() override
        {
            if (this->mission)
            {
                return true;
            }
            return this->tryBuildMission(nullptr);
        }

        bool wait(BaseTask *toWait) override
        {
            this->deleteMission();
            auto preTask = dynamic_cast<MoveToCellTask *>(toWait);
            if (!preTask)
            {
                // only wait the same task type.
                return false;
            }
            this->tryBuildMission(preTask);
            return true;
        }

        bool step(float time) override
        {
            if (!this->mission)
            {
                if (!this->tryBuildMission(nullptr))
                {
                    return false;
                }
            }
            return mission->step(time);
        }

        bool tryResolveOwnerCell(CellKey &cKey, Vector2 &actorPosIn2D)
        {
            Cell::Center *cells = Context<Cell::Center *>::get();

            // check if this state's position on the target cell
            Vector3 aPos3 = this->state->getSceneNode()->getPosition();
            Node2D *root2D = cells->getRoot2D();
            actorPosIn2D = root2D->to2D(aPos3);
            Cell::Instance cell;
            // bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aCellKey);
            bool hitCell = Context<Cell::Center *>::get()->findCellByWorldPosition(aPos3, cell);
            if (!hitCell)
            {
                return false;
            }
            cKey = cell.cKey;
            return true;
        }

        PathFollow2 *tryBuildPath(MoveToCellTask *preTask, PathState *&pathStateRef)
        {

            CellKey aCellKey;
            Vector2 actorPosIn2D;

            if (preTask)
            {
                aCellKey = preTask->cKey2;
                Cell::Center *cells = Context<Cell::Center *>::get();
                actorPosIn2D = cells->getCell(aCellKey).getOrigin2D();
            }
            else
            {

                if (!this->tryResolveOwnerCell(aCellKey, actorPosIn2D))
                {
                    return nullptr;
                }
            }

            std::vector<Vector2> pathByPoint2DNom = Context<CostMap*>::get()->findPath(aCellKey, cKey2);

            std::vector<Vector2> pathByCellCenterIn2D;

            // CellUtil::translatePathToCellCenter(pathByKey, pathByPosition, CellUtil::offset(costMap));

            // Context<Node2D*>::get()->

            std::vector<Vector2> pathByPointIn2D = Context<Cell::Center *>::get()->getCellPointListByNomPoints(pathByPoint2DNom);
            // float pathSpeed = this->Context<Var<float>::Bag*>::get()->getVarVal(".pathSpeed", 1.0f);

            float pathSpeed = Context<Var<float>::Bag *>::get()->getVarVal(".pathSpeed", 1.0f);
            PathFollow2 *path = new PathFollow2(actorPosIn2D, pathByPointIn2D, pathSpeed);
            pathStateRef = new PathState();
            pathStateRef->init();
            pathStateRef->setPath(pathByPoint2DNom, aCellKey, cKey2);
            return path;
        }

        bool tryBuildMission(MoveToCellTask *preTask)
        {

            PathState *pathStateRef;
            PathFollow2 *path2D = tryBuildPath(preTask, pathStateRef);
            if (!path2D)
            {
                return false;
            }

            AnimationStateSet *anisSet = state->getAllAnimationStates();
            if (!anisSet)
            {
                throw std::runtime_error("no animation set, not supported for now.");
            }
            // float aniSpeed = this->Context<Var<float>::Bag*>::get()->getVarVal(".aniSpeed", 1.0f);
            float aniSpeed = Context<Var<float>::Bag *>::get()->getVarVal(".aniSpeed", 1.0f);

            // new child state.
            PathFollow2MissionState *mission = new PathFollow2MissionState(path2D, anisSet, state->getAnimationNames(), aniSpeed, state->getActorHighOffset()); //
            mission->init();
            // delete missionState;
            // this->addChild(missionState);
            this->state->addChild(mission);
            this->mission = mission;
            this->pathState = pathStateRef;
            this->pathFollow2 = path2D;
            return true;
        }

    }; //
};