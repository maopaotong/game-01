// main.cpp - Complete Ogre A* Hex Grid Visualization System
#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <algorithm>
#include <functional>

// === Include OgreBites for modern initialization ===
#include <Bites/OgreApplicationContext.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreFrameListener.h>
#include <OgreRTShaderSystem.h>
#include <OgreTechnique.h>
#include <OgreRenderWindow.h>
#include <iostream>
#include "fg/util/CellMark.h"
#include "fg/util/CellUtil.h"
#include "fg/CoreMod.h"
#include "fg/Master.h"
#include "fg/core/MoveToCellTask.h"
#include "fg/CellInstanceManager.h"

namespace fog
{
    using namespace OgreBites;
    using namespace Ogre;
    // === Custom hash function ===
    //
    // === Input handler for closing application ===
    class HoverCellController : public OgreBites::InputListener
    {

    private:
        CellKey cKey;
        CellInstanceState *preCis;

    public:
        HoverCellController() : preCis(nullptr)
        {
        }

        CONSUMED mouseMoved(const MouseMotionEvent &evt) override
        {
            int x = evt.x;
            int y = evt.y;

            Viewport *viewport = Context<CoreMod *>::get()->getViewport();
            Camera *camera = Context<CoreMod *>::get()->getCamera();

            float ndcX = x / (float)viewport->getActualWidth();
            float ndcY = y / (float)viewport->getActualHeight();

            Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

            Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0); // Y = 0

            auto hitGrd = ray.intersects(ground);

            Ogre::Vector3 pos2;

            if (!hitGrd.first)
            {
                return false;
            }

            pos2 = ray.getPoint(hitGrd.second);
            CellInstanceManager *cellInstMgrState = Context<CellInstanceManager *>::get();
            CellInstanceState *cis = cellInstMgrState->getCellInstanceStateByPosition(pos2);
            if (!cis)
            {
                return false;
            }
            if (cis == preCis)
            {
                return false;
            }
            if (preCis)
            {
                preCis->unsetColour();
            }
            cis->setColour(ColourValue::Red);
            preCis = cis;

            return false;
        }
    };
}; // end of namespace