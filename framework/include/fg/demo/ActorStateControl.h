#pragma once
#include <Ogre.h>
#include <OgreSceneManager.h>
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/core/ActorState.h"
#include "fg/Core.h"
#include "fg/Terrains.h"

namespace fog{
using namespace Ogre;

class ActorStateControl : public ActorState
{


    CostMap *costMap;
   
public:
    ActorStateControl(CostMap *costMap, Core *core) : ActorState(costMap, core)
    {
        
    }
    void create(SceneManager *sMgr, Entity *&entity, SceneNode *&node) override
    {

        entity = sMgr->createEntity("Sinbad.mesh");
        entity->setQueryFlags(0x00000001);
        
        sceNode = sMgr->getRootSceneNode()->createChildSceneNode("SinbadNode");
        sceNode->setScale(*actorScaleVptr, *actorScaleVptr, *actorScaleVptr);
        sceNode->attachObject(entity);
       
        // todo collect auto
    }
};
};//end of namespace