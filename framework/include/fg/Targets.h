#pragma once
#include <string>
#include <vector>
#include <typeindex>

#include <vector>
#include <memory>
#include <type_traits>
#include <functional>
#include <utility>
#include <Ogre.h>
#include <fg/Cell.h>
#include <fg/Context.h>
#include "OgreSceneNode.h"
#include "fg/util/CostMap.h"
#include <fg/Global.h>
#include "fg/PathFollow2.h"
#include "fg/core/PathState.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/Tasks.h"
#include "fg/Cell.h"
namespace fog
{
    class Targets
    {
    public:
        struct MoveToCell : public Tasks::Target
        {

            CellKey cKey;
            MoveToCell(CellKey cKey) : cKey(cKey)
            {
            }
        };
    }; //
};