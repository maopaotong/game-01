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
#include "fg/MaterialNames.h"
#include "stb_truetype.h"
#include <OgreTextureManager.h>
#include <OgreHardwarePixelBuffer.h>
#include "fg/core/Tiles.h"

namespace fog
{
    // === Custom hash function ===
    //
    // === Hexagonal Map Visualizer class ===
    using namespace Ogre;
    using namespace OgreBites;

    class TextureFactory
    {
    private:
        static void createTexture(RenderSystem *renderSystem, Tiles::Terrains )
        {
            
            
            
        }

    public:
        static void createTextures(MaterialManager *matMgr)
        {
           
        }
    };

}; // end of namespace