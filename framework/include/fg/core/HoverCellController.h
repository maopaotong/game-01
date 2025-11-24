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
#include "fg/HoverCellState.h"
#include "fg/core/PathFindTask.h"

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
    public:
        HoverCellController()
        {
        }

        bool mouseMoved(const MouseMotionEvent &evt) override
        {
            int mx = evt.x;
            int my = evt.y;
            Context<HoverCellState *>::get()->onMouseMoving(mx,my);

            Context<State *>::get()->forEach([](State *state)
                                             {
                                                 if (state->isActive() && state->slot(2)->empty())
                                                 {
                                                    state->slot(2)->push([state]( )
                                                     {
                                                         return new PathFindTask(state); //
                                                     });                                                    
                                                 }
                                                 return true; //
                                             });

            return false;
        };
    };
}; // end of namespace