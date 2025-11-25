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
            Cell::Center *cells = Context<Cell::Center>::get();

            // check if this state's position on the target cell
            Vector3 aPos3 = this->movingState->getSceneNode()->getPosition();
            Node2D *root2D = cells->getRoot2D();
            Vector2 actorPosIn2D = root2D->to2D(aPos3);
            Cell::Instance cell;
            // bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aCellKey);
            bool hitCell = Context<Cell::Center>::get()->findCellByWorldPosition(aPos3, cell);
            // todo: not hit?
            return {cell.cKey, actorPosIn2D};
        }

        PathFollow2 buildPath()
        {

            std::tuple<CellKey, Vector2> keyAndPosition = this->resolveOwnerCell();

            std::vector<Vector2> pathByPoint2DNom = Context<CostMap>::get()->findNormPath(std::get<CellKey>(keyAndPosition), cKey2);

            std::vector<Vector2> pathByCellCenterIn2D;

            // CellUtil::translatePathToCellCenter(pathByKey, pathByPosition, CellUtil::offset(costMap));

            // Context<Node2D>::get()->

            std::vector<Vector2> pathByPointIn2D = Context<Cell::Center>::get()->getCellPointListByNomPoints(pathByPoint2DNom);
            // float pathSpeed = this->Context<Var<float>::Bag>::get()->getVarVal(".pathSpeed", 1.0f);

            float pathSpeed = Context<Var<float>::Bag>::get()->getVarVal(".pathSpeed", 1.0f);
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
            // float aniSpeed = this->Context<Var<float>::Bag>::get()->getVarVal(".aniSpeed", 1.0f);
            float aniSpeed = Context<Var<float>::Bag>::get()->getVarVal(".aniSpeed", 1.0f);

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
        State *state;

    public:
        MovingStateManager() : state(nullptr)
        {
            Context<Event::Bus>::get()-> //
                subscribe<EventType, State *>([this](EventType evtType, State *state)
                                              {
                                                  if (evtType == EventType::MovableStatePicked)
                                                  {
                                                      this->state = state;
                                                  }
                                                  else if (evtType == EventType::MovableStateUnpicked)
                                                  {
                                                      this->state = nullptr;
                                                  }
                                                  return true; //
                                              });
        }
        virtual ~MovingStateManager()
        {
        }
        void init() override
        {
        }

        bool movingActiveStateToCellByMousePosition(int mx, int my)
        {
            // normalized (0,1)
            Viewport *viewport = Context<CoreMod>::get()->getViewport();
            Camera *camera = Context<CoreMod>::get()->getCamera();

            float ndcX = mx / (float)viewport->getActualWidth();
            float ndcY = my / (float)viewport->getActualHeight();

            Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

            Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0); // Y = 0
            auto hitGrd = ray.intersects(ground);
            std::cout << "ndc:(" << ndcX << "," << ndcY << ")" << "hit:" << hitGrd.first << std::endl;
            if (!hitGrd.first)
            {
                return false;
            }
            Ogre::Vector3 pos = ray.getPoint(hitGrd.second);

            // bool hitCell = CellUtil::findCellByPoint(costMap, Vector2(pos.x, pos.z), cKey);
            // bool hitCell = CellUtil::findCellByPoint(costMap, Ground::Transfer::to2D(pos), cKey);
            Cell::Instance cell2;
            bool hitCell = Context<Cell::Center>::get()->findCellByWorldPosition(pos, cell2);
            if (!hitCell)
            {
                return false;
            }
            CellKey cKey2 = cell2.cKey;
            // state
            this->movingActiveStateToCell(cKey2);
            // cout << "worldPoint(" << pickX << ",0," << pickZ << "),cellIdx:[" << cx << "," << cy << "]" << endl;
            return true;
        }

        void movingActiveStateToCell(CellKey cKey2)
        {
            if (this->state == nullptr)
            {
                return;
            }

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
            Context<Event::Bus>::get()->emit<EventType, State *>(EventType::MovableStateStartMoving, state);
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
            CellInstanceManager *cisManager = Context<CellInstanceManager>::get();

            for (auto it = this->tasks.begin(); it != this->tasks.end();)
            {
                MoveToCellTask *task = it->get();
                if (!task->step(time))
                {
                    it = this->tasks.erase(it);
                    continue;
                }

                it++;
            }
            return true;
        }

    }; // end of class
}; // end of namespace