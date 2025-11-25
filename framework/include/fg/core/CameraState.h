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
#include "fg/InputState.h"
#include "fg/util/CameraUtil.h"
#include "fg/util/Polygon2.h"
#include "fg/State.h"
#include "fg/demo/InputStateController.h"

namespace fog
{

    using namespace Ogre;

    // === Frame Listener class for main loop ===
    class CameraState : public Ogre::FrameListener, public State
    {
    private:
        bool quit;

    public:
        CameraState() : quit(false)
        {

        }

        // position and orientation of the camera
        bool isViewportInsideGround(Vector3 &position, Quaternion &orientation)
        {
           
            Ogre::Plane gPlane(Ogre::Vector3::UNIT_Y, 0);
            // Ray ray = state->camera->getCameraToViewportRay(0.5f, 0.5f);

            Ogre::Vector3 direction = orientation * Ogre::Vector3::NEGATIVE_UNIT_Z;
            Ogre::Ray ray(position, direction);
            //
            auto hitGrd = ray.intersects(gPlane);
            if (!hitGrd.first)
            {
                return false;
            }
            Vector3 viewCenterOnGround = ray.getPoint(hitGrd.second);
            Cell::Instance cell;
            if(Context<Cell::Center>::get()->findCellByWorldPosition(viewCenterOnGround, cell)){
                return true;
            }

            return false;
        }

        bool frameStarted(const Ogre::FrameEvent &evt) override
        {
            // std::cout << "Frame started!\n";

            Camera *camera = Context<CoreMod>::get()->getCamera();
            // Move camera
            Ogre::SceneNode *node = camera->getParentSceneNode();
            // 获取当前朝向（四元数）
            // Ogre::Quaternion orientation = node->getOrientation();

            // 计算右向量（X轴）
            Ogre::Vector3 right = Ogre::Vector3::UNIT_X;
            Ogre::Vector3 back = Ogre::Vector3::UNIT_Z;

            float speed = 1000.0f;
            Vector3 position = node->getPosition();
            Vector3 step = Ogre::Vector3::ZERO;

            InputStateController * inputState = Context<InputStateController>::get();

            if (inputState->isFront())
            {
                // node->translate(-back * speed * evt.timeSinceLastFrame);
                step += -back * speed * evt.timeSinceLastFrame;
            }
            if (inputState->isBack())
            {
                step += (back * speed * evt.timeSinceLastFrame);
            }
            if (inputState->isLeft())
            {
                step += (-right * speed * evt.timeSinceLastFrame);
            }
            if (inputState->isRight())
            {
                step += (right * speed * evt.timeSinceLastFrame);
            }

            Vector3 position2 = position + step;
            bool validTranlate = true;

            Quaternion orientation = node->getOrientation();
            validTranlate = this->isViewportInsideGround(position2, orientation);

            if (validTranlate)
            {
                node->translate(step);
            }

            return true; // Continue rendering
        }
    };
}; // end of namespace