
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/MaterialNames.h"
#include "fg/util/CostMap.h"
#include "fg/util/HexGridPrinter.h"
#include "fg/util/CellMark.h"
#include "fg/util/DrawerUtil.h"

using namespace Ogre;

#define DEFAULT_HIGH_OFFSET 0.1f

class PathState : public State
{
    Ogre::ManualObject *pathObject;

    std::vector<Ogre::Vector2> currentPath;

    CostMap *costMap;
    CellKey start = CellKey(-1, -1);
    CellKey end = CellKey(-1, -1);

    Core *core;
    float *pathHighOffset;

public:
    PathState(CostMap *costMap, Core *core) 
    {
        this->costMap = costMap;

        Ogre::SceneManager *sceneMgr = core->getSceneManager();
        pathObject = sceneMgr->createManualObject("PathObject");
        this->sceNode = sceneMgr->getRootSceneNode()->createChildSceneNode("PathStateNode");
        this->sceNode->attachObject(pathObject);

        this->pathHighOffset = core->getGlobal()->Var<float>::Bag::createBindVptr(".pathHighOffset", DEFAULT_HIGH_OFFSET, 0.0f, DEFAULT_HIGH_OFFSET * 100.0f);
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
        this->sceNode->setPosition(0.0f, *pathHighOffset, 0.0f);
        this->rebuild();
    }

    void rebuild()
    {

        pathObject->clear();

        // Create path points set for quick lookup
        std::unordered_set<std::pair<int, int>, PairHash> pathSet;
        for (const auto &p : currentPath)
        {
            pathSet.insert({static_cast<int>(p.x), static_cast<int>(p.y)});
        }

        // Begin the manual object
        pathObject->begin(MaterialNames::materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);

        int width = costMap->getWidth();
        int height = costMap->getHeight();

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                // auto vertices = CostMap::calculateVerticesForXZ(x, y, CostMap::hexSize);
                auto vertices = Ground::calculateVertices3D(x, y, costMap, CostMap::hexSize, 1.0f, Global::getTerrainHeightAtPositionWithOffset);

                if (x == start.first && y == start.second)
                {
                    // Start point in green
                    DrawerUtil::drawHexagonTo(pathObject, vertices, Ogre::ColourValue::Green);
                }
                else if (x == end.first && y == end.second)
                {
                    // End point in blue
                    DrawerUtil::drawHexagonTo(pathObject, vertices, Ogre::ColourValue::Blue);
                }
                else if (pathSet.find({x, y}) != pathSet.end())
                {
                    // Path in yellow
                    DrawerUtil::drawHexagonTo(pathObject, vertices, Ogre::ColourValue(1.0f, 1.0f, 0.0f)); // Yellow
                }
            }
        }

        // End the manual object
        pathObject->end();
    }
};
