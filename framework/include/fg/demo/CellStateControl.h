
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/util/DrawerUtil.h"
#include "fg/State.h"
#include "fg/Core.h"
#include "fg/util/CostMap.h"

using namespace Ogre;
#define DEFAULT_CELL_HIGH_OFFSET 0.08f

//
class CellStateControl : public State
{
public:
private:
    Ogre::ManualObject *obj;
    Ogre::SceneNode *node;
    CostMap *costMap;
    float *highOffset;

public:
    CellStateControl(CostMap *costMap, Core *core)
    {
        Ogre::SceneManager *sceneMgr = core->getSceneManager();
        this->costMap = costMap;
        obj = sceneMgr->createManualObject();
        node = sceneMgr->getRootSceneNode()->createChildSceneNode();
        node->attachObject(obj);
        this->highOffset = core->getGlobal()->getVarPtr(".cellHighOffset", DEFAULT_CELL_HIGH_OFFSET, 0.0f, DEFAULT_CELL_HIGH_OFFSET * 100.0f);
        node->setPosition(0, *highOffset, 0);
        //
        buildCellMesh();
    }

    void buildCellMesh()
    {

        obj->clear();

        // Begin the manual object
        obj->begin(MaterialNames::materialNameInUse, Ogre::RenderOperation::OT_TRIANGLE_LIST);
        int width = costMap->getWidth();
        int height = costMap->getHeight();
        float offsetX = width / 2 * CostMap::hexSize;
        float offsetY = height / 2 * CostMap::hexSize;
        Vector2 offset(offsetX, offsetY);

        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                int cost = costMap->getCost(x, y);
                if (cost == CostMap::DEFAULT_COST)
                {
                    continue; // ignore this cell.
                }

                Ogre::ColourValue color = getCostColor(cost);
                // auto vertices = CostMap::calculateVerticesForXZ(x, y, CostMap::hexSize);
                auto vertices = Ground::calculateVertices3D(x, y, costMap, CostMap::hexSize);

                DrawerUtil::drawHexagonTo(obj, vertices, color);
            }
        }

        // End the manual object
        obj->end();
    }

    // Get color based on cost
    Ogre::ColourValue getCostColor(int cost) const
    {
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

    void drawHexagonRing(Ogre::ManualObject *obj,
                         const std::vector<Ogre::Vector3> &verticesInner,
                         const std::vector<Ogre::Vector3> &verticesOuter,
                         const Ogre::ColourValue &colorInner,
                         Ogre::ColourValue &colorOuter)
    {
        const float nomX = 0;
        const float nomY = 1;
        const float nomZ = 0;
        int baseIndex = obj->getCurrentVertexCount();
        for (int i = 0; i < 6; i++)
        {

            obj->position(verticesInner[i]);
            obj->normal(nomX, nomY, nomZ);
            obj->colour(colorInner);

            obj->position(verticesOuter[i]);
            obj->normal(nomX, nomY, nomZ);
            obj->colour(colorOuter);
        }

        // Triangles
        for (int i = 0; i < 6; ++i)
        {
            int p1 = baseIndex + i * 2;
            int p2 = p1 + 1;
            int p3 = baseIndex + ((i + 1) % 6) * 2 + 1;
            int p4 = p3 - 1;

            obj->triangle(p1, p2, p3);
            obj->triangle(p1, p3, p4);
        }
    }
};
