
#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "VarBag.h"
#include "fg/Terrains.h"
#include "fg/Actor.h"
#include "fg/Event.h"
#include "fg/Context.h"
#include "fg/Cell.h"

#include <string>
namespace fog
{
    typedef Event<Actor *, std::string &>::Center ActorPropEC;

    class Global : public Var<float>::Bag,          //
                   public Var<long>::Bag,           //
                   public Var<Vector3>::Bag,        //
                   public Var<Actor *>::Bag,        //
                   public Context<Terrains *>,      //
                   public Context<ActorPropEC *>,   //
                   public Context<Cell::Center **>, //
                   public Context<Node2D *>        //
    {
        typedef void (*FloatVarVistFunc)(const std::string name, float *vPtr, Var<float>::Range *range);

    public:
        static float getTerrainHeightAtPositionWithOffset(float x, float y, float offset = 0.0f)
        {

            return 0.0f;
        }
    };
};