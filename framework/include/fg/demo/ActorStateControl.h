#pragma once
#include <Ogre.h>
#include <OgreSceneManager.h>
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/core/ActorState.h"
#include "fg/Core.h"
#include "fg/Terrains.h"

namespace fog
{
    using namespace Ogre;

    class ActorStateControl : public ActorState
    {

        CostMap *costMap;

    public:
        ActorStateControl(std::string name, CostMap *costMap, Core *core) : ActorState(name, costMap, core)
        {
        }
        void create(SceneManager *sMgr, Entity *&entity, SceneNode *&node) override
        {

            entity = sMgr->createEntity("Sinbad.mesh");
            entity->setQueryFlags(0x00000001);

            sceNode = sMgr->getRootSceneNode()->createChildSceneNode();
            sceNode->setScale(*actorScaleVptr, *actorScaleVptr, *actorScaleVptr);
            sceNode->attachObject(entity);

            // todo collect auto
        }
        virtual void init() override
        {
            ActorState::init();
        }
        virtual AnimationStateSet *getAllAnimationStates()
        {
            return entity->getAllAnimationStates();
        }
        virtual std::vector<std::string> getAnimationNames()
        {
            return this->aniNames;
        }
        virtual float getActorHighOffset()
        {
            return this->actorHighOffset;
        }
    };
}; // end of namespace