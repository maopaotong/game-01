#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/core/EntityState.h"
namespace fog
{

    class MovableStateManager : public State
    {
        State *currentPicked;

    public:
        MovableStateManager() : currentPicked(nullptr)
        {
        }
        virtual ~MovableStateManager()
        {
        }
        void init() override
        {
            EntityState *actor1 = new EntityState("actor1");
            actor1->init();
            this->addChild(actor1);

            EntityState *actor2 = new EntityState("actor2");
            actor2->init();
            this->addChild(actor2);
        }

        State *pick(Ray &ray)
        {

            // 创建射线查询对象
            Ogre::RaySceneQuery *rayQuery = Context<CoreMod *>::get()->getSceneManager()->createRayQuery(ray);
            rayQuery->setSortByDistance(true);  // 按距离排序（最近的优先）
            rayQuery->setQueryMask(0x00000001); // 与 Entity 的查询掩码匹配

            // 执行查询
            Ogre::RaySceneQueryResult &result = rayQuery->execute();

            State *state = nullptr;
            // 遍历结果
            for (auto &it : result)
            {
                Node *node = it.movable->getParentNode();
                State *s = State::get(node);
                if (s && s->pickable())
                {
                    state = s;
                    break;
                }
            }
            Context<CoreMod *>::get()->getSceneManager()->destroyQuery(rayQuery);

            if (currentPicked && currentPicked != state)
            {
                currentPicked->setActive(false);
                Context<Event::Bus*>::get()->emit<EventType, State*>(EventType::MovableStateUnpicked, currentPicked);
            }
            currentPicked = state;
            Context<Event::Bus*>::get()->emit<EventType, State*>(EventType::MovableStatePicked, state);
            return state;
            // high light the cell in which the actor stand.
        }

    }; // end of class
}; // end of namespace