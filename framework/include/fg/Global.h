
#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "VarBag.h"
#include "fg/Terrains.h"
#include "fg/Event.h"
#include "fg/Context.h"
#include "fg/Cell.h"

#include <string>
namespace fog
{

    class Global : public Var<float>::Bag,          //
                   public Var<long>::Bag,           //
                   public Var<Vector3>::Bag        //

                  
    {

    public:
        
    };
};