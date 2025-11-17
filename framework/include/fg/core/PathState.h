
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/MaterialNames.h"
#include "fg/util/CostMap.h"
#include "fg/util/HexGridPrinter.h"
#include "fg/util/CellMark.h"
#include "fg/util/DrawerUtil.h"
#include "fg/core/CellStateBase.h"

namespace fog
{
    using namespace Ogre;

#define DEFAULT_HIGH_OFFSET 1.1f

    class PathState : public CellStateBase
    {
        std::vector<Ogre::Vector2> currentPath;
        CostMap *costMap;
        CellKey start = CellKey(-1, -1);
        CellKey end = CellKey(-1, -1);
        std::unordered_set<std::pair<int, int>, PairHash> pathSet;

    public:
        PathState(CostMap *costMap, Core *core) : CellStateBase(core), costMap(costMap)
        {
        }

        void clearPath()
        {
            this->setPath({}, CellKey(-1, -1), CellKey(-1, -1));
        }

        bool getStart(CellKey &start)
        {
            if (this->start.first == -1)
            {
                return false;
            }
            start = this->start;
            return true;
        }

        void setPath(const std::vector<Ogre::Vector2> &path, CellKey ck1, CellKey ck2)
        {
            currentPath = path;
            start = ck1;
            end = ck2;
            pathSet.clear();
            for (const auto &p : currentPath)
            {
                pathSet.insert({static_cast<int>(p.x), static_cast<int>(p.y)});
            }
            this->rebuildMesh();
        }

        virtual CellKey getDestinationCell()
        {
            return this->end;
        }

        void rebuildMesh() override
        {
            obj->clear();
            // Begin the manual object
            obj->begin(this->material, Ogre::RenderOperation::OT_TRIANGLE_LIST);
            Cell::Center *cc = Global::Context<Cell::Center *>::get();

            MeshBuild::PointOnCircle buildMesh;
            cc->forEachCell([this, &buildMesh](Cell::Instance &cell)
                            {
                                ColourValue color;
                                bool ok = this->resolveColor(cell, color);
                                if (ok)
                                {
                                    buildMesh(this->obj, cell, color);
                                } //
                            });

            // End the manual object
            obj->end();
        }

        bool resolveColor(Cell::Instance &cell, ColourValue &color)
        {

            bool draw = false;
            int x = cell.cKey.first;
            int y = cell.cKey.second;
            if (x == start.first && y == start.second)
            {
                // Start point in green
                color = Ogre::ColourValue::Green;
                draw = true;
            }
            else if (x == end.first && y == end.second)
            {
                // End point in blue
                color = Ogre::ColourValue::Blue;
                draw = true;
            }
            else if (pathSet.find({x, y}) != pathSet.end())
            {
                color = ColourValue(1.0f, 1.0f, 0.0f);
                // Path in yellow
                draw = true;
            }
            return draw;
        }
    };

}; // end of namespace