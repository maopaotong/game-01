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

    public:
        MoveToCellTask(State *state, CellKey cKey2) : state(state), cKey2(cKey2)
        {
        }
        virtual ~MoveToCellTask()
        {
        }
        State *getState()
        {
            return this->state;
        }
        CellKey getTargetCellKey()
        {
            return this->cKey2;
        }
        bool pause() override
        {
            return false;
        }

        bool cancel() override
        {

            return true;
        }

        bool resume() override
        {
            return true;
        }

        bool step(float time) override
        {
            if (!this->mission)
            {
                buildMission(nullptr);
            }
            return mission->step(time);
        }

        std::tuple<CellKey, Vector2> resolveOwnerCell()
        {
            Cell::Center *cells = Context<Cell::Center *>::get();

            // check if this state's position on the target cell
            Vector3 aPos3 = this->state->getSceneNode()->getPosition();
            Node2D *root2D = cells->getRoot2D();
            Vector2 actorPosIn2D = root2D->to2D(aPos3);
            Cell::Instance cell;
            // bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aCellKey);
            bool hitCell = Context<Cell::Center *>::get()->findCellByWorldPosition(aPos3, cell);
            // todo: not hit?
            return {cell.cKey, actorPosIn2D};
        }

        PathFollow2 buildPath()
        {

            std::tuple<CellKey, Vector2> keyAndPosition = this->resolveOwnerCell();

            std::vector<Vector2> pathByPoint2DNom = Context<CostMap *>::get()->findPath(std::get<CellKey>(keyAndPosition), cKey2);

            std::vector<Vector2> pathByCellCenterIn2D;

            // CellUtil::translatePathToCellCenter(pathByKey, pathByPosition, CellUtil::offset(costMap));

            // Context<Node2D*>::get()->

            std::vector<Vector2> pathByPointIn2D = Context<Cell::Center *>::get()->getCellPointListByNomPoints(pathByPoint2DNom);
            // float pathSpeed = this->Context<Var<float>::Bag*>::get()->getVarVal(".pathSpeed", 1.0f);

            float pathSpeed = Context<Var<float>::Bag *>::get()->getVarVal(".pathSpeed", 1.0f);
            PathFollow2 path(std::get<Vector2>(keyAndPosition), pathByPointIn2D, pathSpeed);
            PathState *pathState = new PathState();
            pathState->init();
            pathState->setPath(pathByPoint2DNom, std::get<CellKey>(keyAndPosition), cKey2);

            // TODO add pathState to another common state.
            this->addChild(pathState); //

            return path;
        }

        void buildMission(MoveToCellTask *preTask)
        {

            PathFollow2 path2D = buildPath();

            AnimationStateSet *anisSet = state->getAllAnimationStates();
            if (!anisSet)
            {
                throw std::runtime_error("no animation set, not supported for now.");
            }
            // float aniSpeed = this->Context<Var<float>::Bag*>::get()->getVarVal(".aniSpeed", 1.0f);
            float aniSpeed = Context<Var<float>::Bag *>::get()->getVarVal(".aniSpeed", 1.0f);

            // new child state.
            mission = new PathFollow2MissionState(this->state, path2D, anisSet, state->getAnimationNames(), aniSpeed, state->getActorHighOffset()); //
            mission->init();
            // delete missionState;
            // this->addChild(missionState);
            this->addChild(mission);
        }

    }; //
};