#pragma once

#include <Ogre.h>
#include "fg/PathFollow2.h"
#include "fg/util/CellUtil.h"
#include "fg/util/CostMap.h"
#include "fg/State.h"
#include "PathState.h"
#include "fg/Pickable.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/util/CollectionUtil.h"
#include "fg/Movable.h"
#include "fg/Actor.h"

namespace fog
{
    using namespace Ogre;

    class ActorState : public State, public Pickable, public Ogre::FrameListener, public Movable
    {

        constexpr static float ACTOR_SCALE = 5.0f;
        constexpr static float ACTOR_HEIGHT = 10.0f;

    protected:
        PathFollow2 *pathFolow = nullptr;
        Ogre::Entity *entity;
        CostMap *costMap;
        PathState *pathState;

        PathFollow2MissionState *mission = nullptr;
        std::vector<std::string> aniNames = {"RunBase", "RunTop"};
        Global *global;
        float *actorScaleVptr;
        float *actorHighVptr;
        float actorHighOffset = 0.0f;

    public:
        ActorState(CostMap *costMap, Core *core)
        {
            this->global = core->getGlobal();

            this->costMap = costMap;

            this->actorScaleVptr = core->getGlobal()->Var<float>::Bag::createBindVptr(".actorScale", ACTOR_SCALE, 0.0f, ACTOR_SCALE * 3);
            this->actorHighVptr = core->getGlobal()->Var<float>::Bag::createBindVptr(".actorHighVptr", ACTOR_HEIGHT, 0.0f, ACTOR_HEIGHT * 10);
            this->actorHighOffset = *this->actorHighVptr / 2.0f * *actorScaleVptr;

            pathState = new PathState(costMap, core);
            pathState->init();

            this->setPickable(this);
            this->setFrameListener(this);
            this->setMovable(this);
        }

        void init(Core *core)
        {
            SceneManager *sMgr = core->getSceneManager();
            this->create(sMgr, this->entity, this->sceNode);
            this->setSceneNode(sceNode);
        }

        virtual void create(SceneManager *sMgr, Entity *&entity, SceneNode *&node) = 0;

        void setEntity(Ogre::Entity *entity)
        {
            this->entity = entity;
        }
        Ogre::Entity *getEntity()
        {
            return this->entity;
        }

        PathFollow2 *getPath()
        {
            return this->pathFolow;
        }
        void setPath(PathFollow2 *path)
        {
            this->pathFolow = path;
        }
        bool pickUp(MovableObject *actorMo) override
        {
            // cout << "ActorState::afterPick" << endl;

            SceneNode *node = actorMo->getParentSceneNode();
            const Vector3 &actorPosition = node->getPosition();
            std::cout << "actor.pos:" << actorPosition << "" << std::endl;
            CellKey cKey;
            bool hitActorCell = CellUtil::findCellByPoint(costMap, Ground::Transfer::to2D(actorPosition), cKey);
            ActorState *actor = this;
            if (hitActorCell)
            {
                bool active = this->isActive();
                if (!active)
                {
                    actor->setActive(true);
                }
                else
                {
                    actor->setActive(false);
                    actor->setPath(nullptr);
                    CellKey start;
                    if (this->pathState->getStart(start))
                    {
                        //    markStateControls[MarkType::ACTIVE]->mark(start, false);
                        this->pathState->clearPath();
                    }
                }
            }
            return this->active;
        }

        bool setTargetCell(CellKey &cKey2) override
        {
            if (!this->isActive())
            {
                return false;
            }
            
            Cell::Center* cells = Global::Context<Cell::Center*>::get() ;

            // check if this state's position on the target cell
            Vector3 aPos3 = this->sceNode->getPosition();
            Node2D * root2D = cells->getRoot2D();
            Vector2 actorPosIn2D = root2D->to2D(aPos3);
            Cell::Instance cell;
            //bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aCellKey);
            bool hitCell = Global::Context<Cell::Center*>::get()->findCellByPoint(aPos3, cell);
            if (hitCell)
            {
                CellKey aCellKey = cell.cKey;
                std::vector<Vector2> pathByPoint2DNom = costMap->findPath(cell.cKey, cKey2);


                std::vector<Vector2> pathByCellCenterIn2D;

                //CellUtil::translatePathToCellCenter(pathByKey, pathByPosition, CellUtil::offset(costMap));

                //Global::Context<Node2D*>::get()->

                std::vector<Vector2> pathByPointIn2D = Global::Context<Cell::Center*>::get()->getCellPointListByNomPoints(pathByPoint2DNom);
                float pathSpeed = this->global->Var<float>::Bag::getVarVal(".pathSpeed", 1.0f);

                PathFollow2 *path = new PathFollow2(actorPosIn2D, pathByPointIn2D, pathSpeed);
                this->setPath(path);
                pathState->setPath(pathByPoint2DNom, aCellKey, cKey2);
                AnimationStateSet *anisSet = entity->getAllAnimationStates();
                float aniSpeed = this->global->Var<float>::Bag::getVarVal(".aniSpeed", 1.0f);
                // new child state.
                PathFollow2MissionState *missionState = new PathFollow2MissionState(global, path, anisSet, aniNames, aniSpeed, this->actorHighOffset); //
                // delete missionState;

                if (this->mission)
                {
                    // int size = this->children->size();
                    this->removeChild(this->mission);
                    // std::cout << "children:size" << size << ",after remove child size:" << this->children->size() << std::endl;

                    delete this->mission;
                }
                this->addChild(missionState);
                this->mission = missionState;
            }

            return true;
        }
        virtual CellKey getDestinationCell() override
        {
            return this->pathState->getDestinationCell();
        }
        bool frameStarted(const FrameEvent &evt) override
        {
            void (*func)(State *, const FrameEvent &evt) = [](State *cState, const FrameEvent &evt)
            {
                FrameListener *fl = cState->getFrameListener();
                if (fl)
                {
                    fl->frameStarted(evt);
                }
            };
            this->forEachChild<const FrameEvent &>(func, evt);
            return true;
        }
    };
}; // end of namespace
