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
namespace fog
{
    class MoveToCell
    {
    public:
        
        class Owner : public Tasks::Owner, public Tasks::Task
        {
        public:
            Owner(long mask) : Tasks::Owner(mask), Tasks::Task(nullptr, nullptr)
            {
                this->doPush(this);
            }

            std::vector<std::string> aniNames = {"RunBase", "RunTop"};
            SceneNode *sceNode;
            Entity *entity;
            float actorHighOffset;
            State *state;

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

        class Task : public Tasks::Task
        {

            Owner *owner;
            Targets::MoveToCell *target;

        protected:
            Core *core;
            CostMap *costMap;
            Global *global;
            CellKey cKey2;
            //
            PathFollow2MissionState *mission = nullptr;
            bool failed = false;
            PathState *pathState;
            PathFollow2 *pathFollow2;

        public:
            Task(Targets::MoveToCell *target, Owner *owner, CostMap *costMap, Global *global, Core *core) : core(core), costMap(costMap), global(global), cKey2(target->cKey),
                                                                                               target(target), owner(owner), Tasks::Task(target, owner)
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
                    owner->state->removeChild(this->mission);
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

            bool wait(Tasks::Task *toWait) override
            {
                this->deleteMission();
                auto preTask = dynamic_cast<MoveToCell::Task *>(toWait);
                if (!preTask)
                {
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
                Vector3 aPos3 = this->owner->sceNode->getPosition();
                Node2D *root2D = cells->getRoot2D();
                actorPosIn2D = root2D->to2D(aPos3);
                Cell::Instance cell;
                // bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aCellKey);
                bool hitCell = Context<Cell::Center *>::get()->findCellByWorldPosiion(aPos3, cell);
                if (!hitCell)
                {
                    return false;
                }
                cKey = cell.cKey;
                return true;
            }

            PathFollow2 *tryBuildPath(MoveToCell::Task *preTask, PathState *&pathStateRef)
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

                std::vector<Vector2> pathByPoint2DNom = costMap->findPath(aCellKey, cKey2);

                std::vector<Vector2> pathByCellCenterIn2D;

                // CellUtil::translatePathToCellCenter(pathByKey, pathByPosition, CellUtil::offset(costMap));

                // Context<Node2D*>::get()->

                std::vector<Vector2> pathByPointIn2D = Context<Cell::Center *>::get()->getCellPointListByNomPoints(pathByPoint2DNom);
                float pathSpeed = this->global->Var<float>::Bag::getVarVal(".pathSpeed", 1.0f);

                PathFollow2 *path = new PathFollow2(actorPosIn2D, pathByPointIn2D, pathSpeed);
                pathStateRef = new PathState(core);
                pathStateRef->init();
                pathStateRef->setPath(pathByPoint2DNom, aCellKey, cKey2);
                return path;
            }

            bool tryBuildMission(MoveToCell::Task *preTask)
            {

                PathState *pathStateRef;
                PathFollow2 *path2D = tryBuildPath(preTask, pathStateRef);
                if (!path2D)
                {
                    return false;
                }

                AnimationStateSet *anisSet = owner->entity->getAllAnimationStates();
                float aniSpeed = this->global->Var<float>::Bag::getVarVal(".aniSpeed", 1.0f);
                // new child state.
                PathFollow2MissionState *mission = new PathFollow2MissionState(global, path2D, anisSet, owner->aniNames, aniSpeed, owner->actorHighOffset); //
                mission->init();
                // delete missionState;
                // this->addChild(missionState);
                owner->state->addChild(mission);
                this->mission = mission;
                this->pathState = pathStateRef;
                this->pathFollow2 = path2D;
                return true;
            }
        };

    }; //
};