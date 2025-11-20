#pragma once
#include <string>
#include <vector>
#include <typeindex>

#include <vector>
#include <memory>
#include <type_traits>
#include <functional>
#include <utility>
#include "Task.h"
#include <Ogre.h>
#include "Cell.h"
#include "Context.h"
#include "OgreSceneNode.h"
#include "fg/util/CostMap.h"
#include "Global.h"
#include "fg/PathFollow2.h"
#include "fg/core/PathState.h"
#include "fg/core/PathFollow2MissionState.h"

namespace fog
{

    class MoveToCellTask : public Task
    {
    public:
        class Owner : public Task::Owner
        {
        public:
            std::vector<std::string> aniNames = {"RunBase", "RunTop"};
            SceneNode *sceNode;
            PathState *pathState;
            Entity *entity;
            float actorHighOffset;
            State *state;
        };

    protected:
        CostMap *costMap;
        Global *global;
        CellKey cKey2;
        //
        MoveToCellTask::Owner *owner;
        //
        PathFollow2MissionState *mission = nullptr;
        PathFollow2 *path = nullptr;

    public:
        MoveToCellTask(CostMap *costMap, Global *global, CellKey cKey2) : costMap(costMap), global(global), cKey2(cKey2), Task(std::in_place_type<MoveToCellTask>, std::in_place_type<MoveToCellTask::Owner>)
        {
        }

        bool isDone()
        {
            if (this->mission)
            {

                return this->mission->isDone();
            }
            else
            {
                return false;
            }
        }

        void destroy() override
        {
            if (this->mission)
            {
                owner->state->removeChild(this->mission);
                delete this->mission;
                this->mission = nullptr;
            }
        }

        bool step(long time)
        {
            // todo call mission's step method.
            return false;
        }

        void setOwner(MoveToCellTask::Owner *owner)
        {
            this->owner = owner;
        }

        void setPath(PathFollow2 *path)
        {
            this->path = path;
        }
        void start(Task::Owner *own) override
        {

            this->owner = dynamic_cast<MoveToCellTask::Owner *>(own);
            Cell::Center *cells = Context<Cell::Center *>::get();

            // check if this state's position on the target cell
            Vector3 aPos3 = this->owner->sceNode->getPosition();
            Node2D *root2D = cells->getRoot2D();
            Vector2 actorPosIn2D = root2D->to2D(aPos3);
            Cell::Instance cell;
            // bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aCellKey);
            bool hitCell = Context<Cell::Center *>::get()->findCellByWorldPosiion(aPos3, cell);
            if (hitCell)
            {
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
                this->mission = new PathFollow2MissionState(global, path, anisSet, owner->aniNames, aniSpeed, owner->actorHighOffset); //
                mission->init();

                // delete missionState;
                // this->addChild(missionState);
                owner->state->addChild(mission);
            }
        }
    };

}; //