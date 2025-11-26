#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/core/EntityState.h"
#include "fg/core/Sinbad.h"
#include "fg/core/Tower.h"
#include "fg/InventoryStateManager.h"
namespace fog
{

    class BuildingPlan
    {
    protected:
        CellKey cKey;
        State *building;

    public:
        BuildingPlan(State *building) : building(building)
        {
        }

        void setBuilding(State *state)
        {
            if (this->building)
            {
                delete this->building;
            }
            this->building = state;
        }
        State *getBuilding()
        {
            return this->building;
        }
        virtual ~BuildingPlan()
        {
            if (this->building)
            {
                delete this->building;
                this->building = nullptr;
            }
        }
    };

    class BuildingStateManager : public State
    {
        State *picked;
        BuildingPlan *plan;

        std::unordered_map<CellKey, std::vector<State *>, PairHash> buildingsInCells;

    protected:
        void setPicked(State *picked)
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
        BuildingStateManager() : picked(nullptr), plan(nullptr)
        {
        }
        virtual ~BuildingStateManager()
        {
        }
        void init() override
        {
            Context<Event::Bus>::get()-> //
                subscribe<MouseCellEventType, CellKey>([this](MouseCellEventType type, CellKey cKey)
                                                       {
                                                           if (type == MouseCellEventType::MouseEnterCell)
                                                           {
                                                               if (this->plan)//move the position if has plan
                                                               {
                                                                   this->plan->getBuilding()->findSceneNode()->setPosition(
                                                                       Context<Cell::Center>::get()->getCell(cKey).getOrigin3D());
                                                               }
                                                           }

                                                           return true; //
                                                       });
            Context<Event::Bus>::get()-> //
                subscribe<CellEventType, CellKey>([this](CellEventType type, CellKey cKey)
                                                       {
                                                           if (type == CellEventType::CellAsTarget)
                                                           {
                                                               if (this->plan)//finish the plan by set the position of the building.
                                                               {

                                                                   this->createBuildingAtCell(cKey);
                                                                   delete this->plan;
                                                                   this->plan = nullptr;
                                                               }
                                                           }
                                                           return true; //
                                                       });
        }

        bool pick(Ray &ray) // pick a building.
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
            this->setPicked(picked);
            return picked != nullptr;
        }

        bool planToBuild() // plan a building and waiting mouse cell event to choose a position for the building.
        {
            if (this->plan)
            {
                delete this->plan;
                this->plan = nullptr;
            }
            Tower *tower = new Tower();
            tower->init();
            this->plan = new BuildingPlan(tower);
            return true;
        }

        bool createBuildingAtCell(CellKey cKey)
        {

            auto it = this->buildingsInCells.find(cKey);
            if (it != this->buildingsInCells.end())
            {
                return false;
            }

            if (!Context<InventoryStateManager>::get()->consumeInventory(InventoryType::BuildingPermit, 1.0f))
            {
                return false;
            }

            Tower *tower = new Tower();
            tower->init();
            Cell::Instance ci = Context<Cell::Center>::get()->getCell(cKey);
            Vector3 cisPos = ci.getOrigin3D();
            tower->findSceneNode()->setPosition(cisPos);
            this->addChild(tower);

            this->buildingsInCells[cKey].push_back(tower);
            return true;
        }
    }; // end of class
}; // end of namespace