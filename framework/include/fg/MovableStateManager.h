#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/core/EntityState.h"
namespace fog
{
    class PickedTask : public State, public Stairs
    {
    };

    class MovableStateManager : public State, public Stairs
    {
        State *currentPicked;
        std::vector<std::string> aniNames = {"RunBase", "RunTop"};

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
        bool step(float time) override
        {
            // tracking
            CellInstanceManager *cisManager = Context<CellInstanceManager>::get();
            if (!this->currentPicked)
            {
                return true;
            }
            State *state = this->currentPicked;
            CellInstanceState *currentCis = cisManager->getCellInstanceStateByPosition(state->getSceneNode()->getPosition());

            if (currentCis->getCellKey() != state->getCellKey())
            {
                CellInstanceState *preCis = cisManager->getCellInstanceStateByCellKey(state->getCellKey());
                if (preCis)
                {
                    preCis->unsetColour();
                }

                state->setCellKey(currentCis->getCellKey());

                currentCis->setColour(Ogre::ColourValue::White);
            }

            return true;
        }

        void unpick()
        {
            if (!currentPicked)
            {
                return;
            }
            currentPicked->setActive(false);

            CellInstanceState *cis = Context<CellInstanceManager>::get()->getCellInstanceStateByPosition(currentPicked->getSceneNode()->getPosition());
            cis->unsetColour();
            Context<Event::Bus>::get()->emit<EventType, State *>(EventType::MovableStateUnpicked, currentPicked);
            currentPicked = nullptr;
        }

        void pick(Ray &ray)
        {

            // 创建射线查询对象
            Ogre::RaySceneQuery *rayQuery = Context<CoreMod>::get()->getSceneManager()->createRayQuery(ray);
            rayQuery->setSortByDistance(true);  // 按距离排序（最近的优先）
            rayQuery->setQueryMask(0x00000001); // 与 Entity 的查询掩码匹配

            // 执行查询
            Ogre::RaySceneQueryResult &result = rayQuery->execute();

            State *picked = nullptr;
            // 遍历结果
            for (auto &it : result)
            {
                Node *node = it.movable->getParentNode();
                State *s = State::get(node);
                if (s && s->pickable())
                {
                    picked = s;
                    break;
                }
            }
            Context<CoreMod>::get()->getSceneManager()->destroyQuery(rayQuery);
            if (this->currentPicked == picked)
            {
                return;
            }
            this->unpick();
            if (!picked)
            {
                return;
            }
            currentPicked = picked;
            picked->setActive(true);

            CellInstanceState *cis = Context<CellInstanceManager>::get()->getCellInstanceStateByPosition(picked->getSceneNode()->getPosition());
            cis->setColour(Ogre::ColourValue::White);
            Context<Event::Bus>::get()->emit<EventType, State *>(EventType::MovableStatePicked, picked);
        }

    }; // end of class
}; // end of namespace