#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/CellInstanceManager.h"
#include "fg/core/MoveToCellTask.h"
#include "fg/MovableStateManager.h"
namespace fog
{

    class MoveToCellTask : public State, public Stairs
    {
        std::vector<std::string> aniNames = {"RunBase", "RunTop"};

        float actorHighOffset;

    protected:
        CellKey cKey2; // target cell key.
        State *movingState;
        //
        PathFollow2MissionState *mission = nullptr;
        bool failed = false;

    public:
        MoveToCellTask(State *state, CellKey cKey2) : movingState(state), cKey2(cKey2)
        {
        }
        virtual ~MoveToCellTask()
        {
        }
        State *getState()
        {
            return this->movingState;
        }
        CellKey getTargetCellKey()
        {
            return this->cKey2;
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
            Vector3 aPos3 = this->movingState->getSceneNode()->getPosition();
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
            // PathState *pathState = new PathState();
            // pathState->init();
            // pathState->setPath(pathByPoint2DNom, std::get<CellKey>(keyAndPosition), cKey2);

            // TODO add pathState to another common state.
            // this->addChild(pathState); //

            return path;
        }

        void buildMission(MoveToCellTask *preTask)
        {

            PathFollow2 path2D = buildPath();

            AnimationStateSet *anisSet = movingState->getAllAnimationStates();
            if (!anisSet)
            {
                throw std::runtime_error("no animation set, not supported for now.");
            }
            // float aniSpeed = this->Context<Var<float>::Bag*>::get()->getVarVal(".aniSpeed", 1.0f);
            float aniSpeed = Context<Var<float>::Bag *>::get()->getVarVal(".aniSpeed", 1.0f);

            // new child state.
            mission = new PathFollow2MissionState(this->movingState, path2D, anisSet, movingState->getAnimationNames(), aniSpeed, movingState->getActorHighOffset()); //
            mission->init();
            // delete missionState;
            // this->addChild(missionState);
            this->addChild(mission);
        }

    }; // end of class
    class MovingStateManager : public State, public Stairs
    {
        std::vector<std::unique_ptr<MoveToCellTask>> tasks;

    public:
        MovingStateManager()
        {
        }
        virtual ~MovingStateManager()
        {
        }
        void init() override
        {
        }
        void movingActiveStateToCell(CellKey cKey2)
        {

            // add new tasks
            Context<MovableStateManager *>::get()-> //
                forEach([this, &cKey2](State *state)
                        {
                            if (state->isActive())
                            {
                                this->addTask(state, cKey2);
                            }
                            return true; //
                        });
        }
        void addTask(State *state, CellKey cKey2)
        {
            // clear old task on the same state
            for (auto &it = this->tasks.begin(); it != tasks.end();)
            {
                MoveToCellTask *task = it->get();
                if (task->getState() == state)
                {
                    it = tasks.erase(it);
                }
                else
                {
                    it++;
                }
            }

            //
            MoveToCellTask *task = new MoveToCellTask(state, cKey2);
            this->tasks.push_back(std::unique_ptr<MoveToCellTask>(task));
        }
        template <typename F>
        void forEachTask(F &&f)
        {
            for (auto &task : this->tasks)
            {
                if (!f(task.get()))
                {
                    break;
                }
            }
        }

        GOON step(float time) override
        {
            CellInstanceManager *cisManager = Context<CellInstanceManager *>::get();

            for (auto it = this->tasks.begin(); it != this->tasks.end();)
            {
                MoveToCellTask *task = it->get();
                if (!task->step(time))
                {
                    it = this->tasks.erase(it);
                    continue;
                }

                it++;
                // tracking
                State *state = task->getState();
                CellInstanceState *currentCis = cisManager->getCellInstanceStateByPosition(state->getSceneNode()->getPosition());
                if (currentCis)
                {
                    if (currentCis->getCellKey() != state->getCellKey())
                    {
                        CellInstanceState *preCis = cisManager->getCellInstanceStateByCellKey(state->getCellKey());
                        if (preCis)
                        {
                            preCis->unsetColour();
                        }

                        state->setCellKey(currentCis->getCellKey());

                        currentCis->setColour(Ogre::ColourValue::Green);
                    }
                }
            }
            return true;
        }

    }; // end of class
}; // end of namespace