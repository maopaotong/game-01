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

namespace fog
{
    using namespace Ogre;
    using Vector3Ref = Property::Ref<Vector3>;

    class EntityState : public State
    {

    protected:
        Ogre::Entity *entity;

        std::vector<std::string> aniNames;
        float scale;
        float heigh;
        float heighOffset;

        std::string mesh;

    public:
        EntityState(std::string mesh, float scale, float height, std::vector<std::string> aniNames) : mesh(mesh),
                                                                                                      scale(scale),
                                                                                                      heigh(height),
                                                                                                      aniNames(aniNames)
        {
            this->heighOffset = this->heigh / 2.0f * scale;
        }

        virtual ~EntityState()
        {
        }

        virtual void init() override
        {

            SceneManager *sMgr = Context<CoreMod>::get()->getSceneManager();

            entity = sMgr->createEntity(mesh);
            entity->setQueryFlags(0x00000001);

            sceNode = sMgr->getRootSceneNode()->createChildSceneNode();
            sceNode->setScale(scale, scale, scale);
            sceNode->attachObject(entity);

            this->setSceneNode(sceNode);

            float height = Context<Terrains>::get()->getHeightWithNormalAtWorldPosition(Vector3(0, 0, 0), nullptr);

            sceNode->translate(Vector3(0, height + this->heighOffset, 0));
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
            return this->heighOffset;
        }
    };
}; // end of namespace
