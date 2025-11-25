#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/core/EntityState.h"
namespace fog
{
    class MovingState : public State, public Stairs
    {
    public:
        State *state;
        CellInstanceState *cis;
        MovingState() : state(nullptr), cis(nullptr)
        {
        }

        ~MovingState()
        {
            setState(nullptr);
        }

        GOON step(float time) override
        {
            tryUpdateCis();
            return true;
        }
        void setState(State *state2)
        {
            if (this->state)
            {
                trySetCis(nullptr);
            }
            this->state = state2;
            tryUpdateCis();
            Context<Event::Bus>::get()-> //
                emit<EventType, State *>(state2 ? EventType::MovableStatePicked : EventType::MovableStateUnpicked, state2);
        }
        void tryUpdateCis()
        {
            if (this->state)
            {
                CellInstanceManager *cisManager = Context<CellInstanceManager>::get();
                CellInstanceState *cis2 = cisManager->getCellInstanceStateByPosition(state->getSceneNode()->getPosition());
                this->trySetCis(cis2);
            }
            else
            {
                trySetCis(nullptr);
            }
        }
        void trySetCis(CellInstanceState *cis2)
        {
            if (this->cis == cis2)
            {
                return;
            }
            if (this->cis)
            {
                this->cis->popColour();
            }
            this->cis = cis2;
            if (this->cis)
            {
                this->cis->pushColour(ColourValue::White);
            }
        }
    };

    class MovableStateManager : public State, public Stairs
    {
        MovingState movingState;

        std::vector<std::string> aniNames = {"RunBase", "RunTop"};

    public:
        MovableStateManager()
        {
            Context<Event::Bus>::get()-> //
                subscribe<EventType, State *>([this](EventType evtType, State *state)
                                              {
                                                  if (evtType == EventType::MovableStateStartMoving)
                                                  {
                                                      this->movingState.setState(nullptr);
                                                  }
                                                  return true; //
                                              });
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
            movingState.step(time);
            return true;
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
            this->movingState.setState(picked);
        }

    }; // end of class
}; // end of namespace