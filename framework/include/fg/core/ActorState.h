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
#include "fg/MoveToCell.h"

namespace fog
{
    using namespace Ogre;
    using Vector3Ref = Property::Ref<Vector3>;
    class ActorState : public State, public Pickable, public Ogre::FrameListener, public Actor
    {

        constexpr static float ACTOR_SCALE = 5.0f;
        constexpr static float ACTOR_HEIGHT = 10.0f;

    protected:
        Ogre::Entity *entity;
        CostMap *costMap;

        PathFollow2MissionState *mission = nullptr;
        std::vector<std::string> aniNames = {"RunBase", "RunTop"};
        Global *global;
        float *actorScaleVptr;
        float *actorHighVptr;
        float actorHighOffset = 0.0f;

        Vector3Ref position;

        Core *core;

        class MoveToCellTaskOwner : public MoveToCell::Owner
        {
            Global *global;
            CostMap *costMap;
            Core * core;
        public:
            MoveToCellTaskOwner(Global *global, CostMap *costMap,Core* core) : MoveToCell::Owner(1), costMap(costMap), global(global),core(core)
            {
            }
            bool tryTakeTarget(Tasks::Target *target) override
            {
                this->pushOrWait(new MoveToCell::Task(static_cast<MoveToCell::Target *>(target), this, costMap, global,core));
                //
                return true;
            }
        };

    public:
        ActorState(CostMap *costMap, Core *core) : core(core)
        {
            this->global = core->getGlobal();

            this->costMap = costMap;

            this->actorScaleVptr = core->getGlobal()->Var<float>::Bag::createBindVptr(".actorScale", ACTOR_SCALE, 0.0f, ACTOR_SCALE * 3);
            this->actorHighVptr = core->getGlobal()->Var<float>::Bag::createBindVptr(".actorHighVptr", ACTOR_HEIGHT, 0.0f, ACTOR_HEIGHT * 10);
            this->actorHighOffset = *this->actorHighVptr / 2.0f * *actorScaleVptr;

            this->setPickable(this);
            this->setFrameListener(this);
        }

        virtual ~ActorState()
        {
        }

        virtual void init() override
        {

            SceneManager *sMgr = core->getSceneManager();
            this->create(sMgr, this->entity, this->sceNode);
            this->setSceneNode(sceNode);

            float height = Context<Terrains *>::get()->getHeightWithNormalAtWorldPosition(Vector3(0, 0, 0), nullptr);

            this->position = this->createProperty("actor.position", Vector3(0, height + this->actorHighOffset, 0));
            sceNode->translate(this->position);
            // init task owner.
            MoveToCell::Owner *owner = new MoveToCellTaskOwner(global, costMap,core);
            owner->actorHighOffset = this->actorHighOffset;
            owner->aniNames = aniNames;
            owner->entity = this->entity;
            owner->sceNode = this->sceNode;
            owner->state = this;

            this->taskOwner = owner;
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

        bool pickUp(MovableObject *actorMo) override
        {
            // cout << "ActorState::afterPick" << endl;

            SceneNode *node = actorMo->getParentSceneNode();
            const Vector3 &actorPosition = node->getPosition();
            std::cout << "actor.pos:" << actorPosition << "" << std::endl;
            // CellKey cKey;

            // bool hitActorCell = CellUtil::findCellByPoint(costMap, Ground::Transfer::to2D(actorPosition), cKey);
            Cell::Instance cell;
            bool hitActorCell = this->findCell(actorPosition, cell);

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
                }
            }
            return this->active;
        }
        bool frameStarted(const FrameEvent &evt) override
        {
            void (*func)(State *, const FrameEvent &evt) = [](State *cState, const FrameEvent &evt)
            {
                FrameListener *fl = cState->getFrameListener();
                if (fl)
                {
                    // fl->frameStarted(evt);
                }
            };
            this->forEachChild<const FrameEvent &>(func, evt);
            return true;
        }
    };
}; // end of namespace
