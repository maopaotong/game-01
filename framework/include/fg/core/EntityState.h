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
    
    class EntityState : public State
    {

        constexpr static float ACTOR_SCALE = 5.0f;
        constexpr static float ACTOR_HEIGHT = 10.0f;

    protected:
        Ogre::Entity *entity;

        PathFollow2MissionState *mission = nullptr;
        std::vector<std::string> aniNames = {"RunBase", "RunTop"};
        float *actorScaleVptr;
        float *actorHighVptr;
        float actorHighOffset = 0.0f;

        Vector3Ref position;

        std::string name;

    public:
        EntityState(std::string name) : name(name)
        {

            this->actorScaleVptr = Context<Var<float>::Bag *>::get()->createBindVptr(name + ".actorScale", ACTOR_SCALE, 0.0f, ACTOR_SCALE * 3);
            this->actorHighVptr = Context<Var<float>::Bag *>::get()->createBindVptr(name + ".actorHighVptr", ACTOR_HEIGHT, 0.0f, ACTOR_HEIGHT * 10);

            this->actorHighOffset = *this->actorHighVptr / 2.0f * *actorScaleVptr;

        }

        ~EntityState()
        {
        }

        virtual void init() override
        {

            SceneManager *sMgr = Context<CoreMod *>::get()->getSceneManager();
           
            entity = sMgr->createEntity("Sinbad.mesh");
            entity->setQueryFlags(0x00000001);

            sceNode = sMgr->getRootSceneNode()->createChildSceneNode();
            sceNode->setScale(*actorScaleVptr, *actorScaleVptr, *actorScaleVptr);
            sceNode->attachObject(entity);

            this->setSceneNode(sceNode);

            float height = Context<Terrains *>::get()->getHeightWithNormalAtWorldPosition(Vector3(0, 0, 0), nullptr);

            this->position = this->createProperty(name + ".actor.position", Vector3(0, height + this->actorHighOffset, 0));
            sceNode->translate(this->position);
            // init task owner.
        }

        Ogre::Entity *getEntity()
        {
            return this->entity;
        }
        AnimationStateSet *getAllAnimationStates()
        {
            return entity->getAllAnimationStates();
        }
        std::vector<std::string> getAnimationNames()
        {
            return this->aniNames;
        }
        float getActorHighOffset()
        {
            return this->actorHighOffset;
        }
    };
}; // end of namespace
