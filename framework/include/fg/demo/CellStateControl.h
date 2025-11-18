
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
#include "fg/MeshBuild.h"
#include "fg/VarBag.h"
namespace fog
{
    using namespace Ogre;

    //
    class CellStateControl : public CellStateBase
    {
    public:
        CostMap *costMap;
        bool showCost0 = true;
        bool showCost1 = true;
        bool showCost2 = true;
        bool showCost3 = true;
        bool showOther = true;

    public:
        CellStateControl(CostMap *costMap, Core *core) : CellStateBase(core), costMap(costMap)
        {
        }
        void init() override
        {
            CellStateBase::init();
        }
        void rebuildMesh() override
        {

            Cell::Center *cc = Global::Context<Cell::Center *>::get();
            // MeshBuild::PointOnCircle buildMesh(obj);
            MeshBuild::SpiderNet buildMesh(obj);
            buildMesh.begin(this->material);
            cc->forEachCell([this, &buildMesh](Cell::Instance &cell)
                            {
                                ColourValue color;
                                bool build = this->getCostColor(cell, color);
                                if(build){
                                    buildMesh(cell, color); //
                                } });
            buildMesh.end();
        }

        // Get color based on cost
        bool getCostColor(Cell::Instance &cell, Ogre::ColourValue &color) const
        {
            const int cost = costMap->getCost(cell.cKey.first, cell.cKey.second);
            switch (cost)
            {
            case CostMap::OBSTACLE:
                color = Ogre::ColourValue::Red;
                return showCost0;
            case CostMap::DEFAULT_COST:
                color = Ogre::ColourValue(0.8f, 0.6f, 0.2f); // light Sand color
                return showCost1;
            case 2:
                color = Ogre::ColourValue(0.6f, 0.4f, 0.1f); // Dark Sand color
                return showCost2;
            case 3:
                color = Ogre::ColourValue(0.2f, 0.4f, 0.8f); // Water color
                return showCost3;
            default:
                color = Ogre::ColourValue(0.7f, 0.7f, 0.7f); // light gray

                return showOther;
            }
        }
    };

}; // end of namespace