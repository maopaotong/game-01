#pragma once
#include <string>
#include <vector>
#include <typeindex>

#include <vector>
#include <memory>
#include <type_traits>
#include <functional>
#include <utility>
#include "Tasks.h"
#include <Ogre.h>
#include "Cell.h"
#include "Context.h"
#include "OgreSceneNode.h"
#include "fg/util/CostMap.h"
#include "Global.h"
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
        struct Target : public Tasks::Target
        {

            CellKey cKey;
            Target(CellKey cKey) : cKey(cKey)
            {
            }
        };

        class Owner : public Tasks::Owner, public Tasks::Task
        {
        public:
            Owner(long mask) : Tasks::Owner(mask), Tasks::Task(nullptr, nullptr)
            {
                this->push(this);
            }

            std::vector<std::string> aniNames = {"RunBase", "RunTop"};
            SceneNode *sceNode;
            PathState *pathState;
            Entity *entity;
            float actorHighOffset;
            State *state;

            bool step(float time) override
            {
                return true;
            }

            void destroy()
            {
                // todo destoy the owner by remove owner from monitor.
            }
        };

        class Task : public Tasks::Task
        {

            Owner *owner;
            Target *target;

        protected:
            CostMap *costMap;
            Global *global;
            CellKey cKey2;
            //
            PathFollow2MissionState *mission = nullptr;
            PathFollow2 *path = nullptr;
            bool failed = false;

        public:
            Task(Target *target, Owner *owner, CostMap *costMap, Global *global) : costMap(costMap), global(global), cKey2(target->cKey),
                                                                                   target(target), owner(owner), Tasks::Task(target, owner)
            {
            }

            void destroy() override
            {
                owner->state->removeChild(this->mission);
                delete this->mission;
            }

            bool step(float time) override
            {
                if (this->failed)
                {
                    return false;
                }
                // todo call mission's step method.
                if (!this->mission)
                {
                    this->mission = this->tryCreateMission();
                    if (!this->mission)
                    {
                        this->failed = true; // failed to create mission.
                        // so stop this task, do not call step next time.
                        return false;
                    }
                }
                return mission->step(time);
            }

            void setPath(PathFollow2 *path)
            {
                this->path = path;
            }

            PathFollow2MissionState *tryCreateMission()
            {

                Cell::Center *cells = Context<Cell::Center *>::get();

                // check if this state's position on the target cell
                Vector3 aPos3 = this->owner->sceNode->getPosition();
                Node2D *root2D = cells->getRoot2D();
                Vector2 actorPosIn2D = root2D->to2D(aPos3);
                Cell::Instance cell;
                // bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aCellKey);
                bool hitCell = Context<Cell::Center *>::get()->findCellByWorldPosiion(aPos3, cell);
                if (!hitCell)
                {
                    return nullptr;
                }
                CellKey aCellKey = cell.cKey;
                std::vector<Vector2> pathByPoint2DNom = costMap->findPath(cell.cKey, cKey2);

                std::vector<Vector2> pathByCellCenterIn2D;

                // CellUtil::translatePathToCellCenter(pathByKey, pathByPosition, CellUtil::offset(costMap));

                // Context<Node2D*>::get()->

                std::vector<Vector2> pathByPointIn2D = Context<Cell::Center *>::get()->getCellPointListByNomPoints(pathByPoint2DNom);
                float pathSpeed = this->global->Var<float>::Bag::getVarVal(".pathSpeed", 1.0f);

                PathFollow2 *path = new PathFollow2(actorPosIn2D, pathByPointIn2D, pathSpeed);
                this->setPath(path);
                owner->pathState->setPath(pathByPoint2DNom, aCellKey, cKey2);
                AnimationStateSet *anisSet = owner->entity->getAllAnimationStates();
                float aniSpeed = this->global->Var<float>::Bag::getVarVal(".aniSpeed", 1.0f);
                // new child state.
                PathFollow2MissionState *mission = new PathFollow2MissionState(global, path, anisSet, owner->aniNames, aniSpeed, owner->actorHighOffset); //
                mission->init();
                // delete missionState;
                // this->addChild(missionState);
                owner->state->addChild(mission);

                return mission;
            }
        };

    }; //
};