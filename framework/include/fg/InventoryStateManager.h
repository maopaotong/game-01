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

    class Inventory : public State
    {
        InventoryType type;
        float capacity;

    public:
        Inventory(InventoryType type) : type(type), capacity(0.0f)
        {
        }
        virtual ~Inventory()
        {
        }

        void add(float amount)
        {
            this->capacity += amount;
        }

        bool consume(float amount)
        {
            if (this->capacity < amount)
            {
                return false;
            }
            this->capacity -= amount;

            return true;
        }

        InventoryType getType() const
        {
            return this->type;
        }
        float getCapacity() const
        {
            return this->capacity;
        }
    };

    class InventoryStateManager : public State
    {

    protected:
    public:
        InventoryStateManager()
        {
        }
        virtual ~InventoryStateManager()
        {
        }
        void init() override
        {
            Inventory *inv = new Inventory(InventoryType::BuildingPermit);
            inv->init();
            inv->add(1.0f); //
            this->addChild(inv);
        }

        template <typename F>
        void forEachInventory(F &&func)
        {
            this->forEachChild([&func](State *state)
                               {
                                   Inventory *inv = dynamic_cast<Inventory *>(state);
                                   if (inv)
                                   {
                                       return func(inv);
                                   }
                                   return true; //
                               });
        }

        bool consumeInventory(InventoryType type, float amount)
        {
            Inventory *inv = this->getInventory(type);
            if (!inv)
            {
                return false;
            }
            return inv->consume(amount);
        }

        Inventory * getInventory(InventoryType type){
            Inventory * result = nullptr;
            this->forEachInventory([&](Inventory * inv){
                if(inv->getType() == type){
                    result = inv;
                    return false; // stop
                }
                return true; // continue
            });
            return result;
        }

    }; // end of class
}; // end of namespace