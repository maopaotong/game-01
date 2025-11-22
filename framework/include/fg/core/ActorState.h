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
    using Vector3Ref = Property::Ref<Vector3>;
    class ActorState : public State, public Pickable, public Actor
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

        std::string name;

    public:
        ActorState(std::string name, CostMap *costMap, Core *core) : name(name), core(core)
        {

            this->costMap = costMap;

            // this->actorScaleVptr = core->getGlobal()->Var<float>::Bag::createBindVptr(".actorScale", ACTOR_SCALE, 0.0f, ACTOR_SCALE * 3);
            // this->actorHighVptr = core->getGlobal()->Var<float>::Bag::createBindVptr(".actorHighVptr", ACTOR_HEIGHT, 0.0f, ACTOR_HEIGHT * 10);
            this->actorScaleVptr = Context<Var<float>::Bag *>::get()->createBindVptr(name + ".actorScale", ACTOR_SCALE, 0.0f, ACTOR_SCALE * 3);
            this->actorHighVptr = Context<Var<float>::Bag *>::get()->createBindVptr(name + ".actorHighVptr", ACTOR_HEIGHT, 0.0f, ACTOR_HEIGHT * 10);

            this->actorHighOffset = *this->actorHighVptr / 2.0f * *actorScaleVptr;

            // this->setFrameListener(this);
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

            this->position = this->createProperty(name + ".actor.position", Vector3(0, height + this->actorHighOffset, 0));
            sceNode->translate(this->position);
            // init task owner.
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
        bool pickable() override
        {
            if (this->active)
            {
                return false;
            }
            return true;
        }

        virtual AnimationStateSet *getAllAnimationStates() = 0;
        virtual std::vector<std::string> getAnimationNames() = 0;
        virtual float getActorHighOffset() = 0;
    };
}; // end of namespace
