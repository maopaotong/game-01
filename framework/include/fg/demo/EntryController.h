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
#include "fg/PathingStateManager.h"
namespace fog
{
    using namespace OgreBites;
    using namespace Ogre;
    // === Custom hash function ===
    //
    // === Input handler for closing application ===
    class EntryController : public OgreBites::InputListener, public FrameListener
    {

    private:
    public:
        EntryController()
        {
        }

        CONSUMED mouseMoved(const MouseMotionEvent &evt) override
        {
            Context<PathingStateManager *>::get()->onMouseMoved(evt.x, evt.y);
            return false;
        }
        GOON frameStarted(const FrameEvent &evt) override
        {
            return Context<PathingStateManager *>::get()->step(evt.timeSinceLastFrame);
        }
    };
}; // end of namespace