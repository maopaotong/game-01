#pragma once
#include <Ogre.h>
#include <OgreSceneManager.h>
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/core/ActorState.h"
#include "fg/Core.h"

using namespace Ogre;

class ActorStateControl : public ActorState
{

    constexpr static float ACTOR_SCALE = 5.0f;
    constexpr static float ACTOR_HEIGHT = 10.0f;

    CostMap *costMap;
    float *actorScaleVptr;
    float *actorHighVptr;

public:
    ActorStateControl(CostMap *costMap, Core *core) : ActorState(costMap, core)
    {
        this->actorScaleVptr = core->getGlobal()->getVarPtr(".actorScale", ACTOR_SCALE, 0.0f, ACTOR_SCALE * 3);
        this->actorHighVptr = core->getGlobal()->getVarPtr(".actorHighVptr", ACTOR_HEIGHT, 0.0f, ACTOR_HEIGHT * 10);
    }
    void create(SceneManager *sMgr, Entity *&entity, SceneNode *&node) override
    {

        entity = sMgr->createEntity("Sinbad.mesh");
        entity->setQueryFlags(0x00000001);
        float actorHighOffset = *this->actorHighVptr / 2.0f * *actorScaleVptr;
        sceNode = sMgr->getRootSceneNode()->createChildSceneNode();
        sceNode->setScale(*actorScaleVptr, *actorScaleVptr, *actorScaleVptr);
        sceNode->attachObject(entity);
        sceNode->translate(0, actorHighOffset, 0);
        // todo collect auto
    }
};