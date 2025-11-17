
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/util/DrawerUtil.h"
#include "fg/State.h"
#include "fg/Core.h"
#include "fg/util/CostMap.h"
#include "fg/Cell.h"
#include "fg/Global.h"
#include "fg/core/CellStateBase.h"
#include "fg/util/CostMap.h"
namespace fog
{
    using namespace Ogre;

    //
    class CellStateControl : public CellStateBase
    {
    public:
        CostMap *costMap;

    public:
        CellStateControl(CostMap *costMap, Core *core) : CellStateBase(core), costMap(costMap)
        {
        }

        virtual void buildInstanceMesh(ManualObject *obj, Cell::Instance &cell)
        {
            ColourValue color = getCostColor(cell);
            cell.buildMesh(obj, color);
        }

        // Get color based on cost
        Ogre::ColourValue getCostColor(Cell::Instance &cell) const
        {
            const int cost = costMap->getCost(cell.cKey.first, cell.cKey.second);
            switch (cost)
            {
            case CostMap::OBSTACLE:
                return Ogre::ColourValue::Red;
            case CostMap::DEFAULT_COST:
                return Ogre::ColourValue(0.8f, 0.6f, 0.2f); // light Sand color
            case 2:
                return Ogre::ColourValue(0.6f, 0.4f, 0.1f); // Dark Sand color
            case 3:
                return Ogre::ColourValue(0.2f, 0.4f, 0.8f); // Water color
            default:
                return Ogre::ColourValue(0.7f, 0.7f, 0.7f); // light gray
            }
        }
    };

}; // end of namespace