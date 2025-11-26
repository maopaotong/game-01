#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/core/EntityState.h"
#include "fg/core/Sinbad.h"
#include "fg/core/Tower.h"

namespace fog
{

    class BuildingStateManager : public State
    {
        State *picked;

    protected:
        void setState(State *picked)
        {
            if (this->picked == picked)
            {
                return;
            }
            if (this->picked)
            {
                // unpick previous
                Context<Event::Bus>::get()-> //
                    emit<BuildingEventType, State *>(BuildingEventType::StateUnpicked, this->picked);
            }
            this->picked = picked;
            if (this->picked)
            {
                // pick new
                Context<Event::Bus>::get()-> //
                    emit<BuildingEventType, State *>(BuildingEventType::StatePicked, this->picked);
            }
        }

    public:
        BuildingStateManager() : picked(nullptr)
        {
        }
        virtual ~BuildingStateManager()
        {
        }
        void init() override
        {

            EntityState *tower = new Tower();
            tower->init();
            this->addChild(tower);
        }

        bool pick(Ray &ray)
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
                if (s && s->pickable() && s->getParent() == this)
                {

                    picked = s;
                    break;
                }
            }
            Context<CoreMod>::get()->getSceneManager()->destroyQuery(rayQuery);
            this->setState(picked);
            return picked != nullptr;
        }
    }; // end of class
}; // end of namespace