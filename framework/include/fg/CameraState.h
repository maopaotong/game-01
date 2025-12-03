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
    class CameraState : public State, public Stairs
    {
        static constexpr float DEFAULT_CAMERA_TOP_DISTANCE = 2 * 1000.0f;
        static constexpr float DEFAULT_CAMERA_HIGH_MIN = 100.0f;
        static constexpr float DEFAULT_CAMERA_HITH_MAX = 1000.0f * 5;
        static constexpr float DEFAULT_CAMERA_ROLL_SPEED = (DEFAULT_CAMERA_HITH_MAX - DEFAULT_CAMERA_HIGH_MIN) / 10.0f;

    private:
        float cameraTopDistance;
        float cameraHighMin;
        float cameraHighMax;
        float cameraRollSpeed;

    private:
        bool quit;

    public:
        CameraState() : quit(false),cameraTopDistance(DEFAULT_CAMERA_TOP_DISTANCE),
                                  cameraHighMin(DEFAULT_CAMERA_HIGH_MIN),
                                  cameraHighMax(DEFAULT_CAMERA_HITH_MAX),
                                  cameraRollSpeed(DEFAULT_CAMERA_ROLL_SPEED)
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

        bool step(float timeSinceLastFrame) override
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

            float speed = FOG_CAM_SPEED;
            Vector3 position = node->getPosition();
            Vector3 step = Ogre::Vector3::ZERO;

            InputStateController * inputState = Context<InputStateController>::get();

            if (inputState->isFront())
            {
                // node->translate(-back * speed * evt.timeSinceLastFrame);
                step += -back * speed * timeSinceLastFrame;
            }
            if (inputState->isBack())
            {
                step += (back * speed * timeSinceLastFrame);
            }
            if (inputState->isLeft())
            {
                step += (-right * speed * timeSinceLastFrame);
            }
            if (inputState->isRight())
            {
                step += (right * speed * timeSinceLastFrame);
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

        bool mouseWheelRolled(const MouseWheelEvent &evt)
        {
            Camera *cam = Context<CoreMod>::get()->getCamera();
            Ogre::SceneNode *node = cam->getParentSceneNode();
            Vector3 translate = Ogre::Vector3::NEGATIVE_UNIT_Y * evt.y * cameraRollSpeed;
            Vector3 posTarget = node->getPosition() + translate;
            if (posTarget.y < this->cameraHighMin)
            {
                posTarget.y = this->cameraHighMin;
            }

            node->setPosition(posTarget);

            Context<Var<Vector3>::Bag>::get()->setVar(".camera.position", posTarget);
            float distance = this->cameraTopDistance;
            // if (distance < posTarget.y)
            // {
            //     distance = posTarget.y + 10;
            // }
            alignHorizonToTop(node, cam, distance);

            return false;
        }
        void alignHorizonToTop(Ogre::SceneNode *camNode, Ogre::Camera *cam, Ogre::Real distance)
        {
            Ogre::Radian fovY = cam->getFOVy();
            Ogre::Real camHeight = camNode->getPosition().y; // 假设地面 Y=0

            // 防止高度 <= 0
            if (camHeight <= 0.1f)
                camHeight = 0.1f;

            // 计算相机到目标点的俯角（从水平线向下）
            Ogre::Radian depressionAngle = Ogre::Math::ATan(camHeight / distance);
            // 目标 pitch：向下转 (depressionAngle + fovY/2)
            Ogre::Radian targetPitch = -(depressionAngle + fovY / 2);

            const Ogre::Radian PITCH_LIMIT = Ogre::Degree(89.9);
            targetPitch = Ogre::Math::Clamp(targetPitch, -PITCH_LIMIT, PITCH_LIMIT);

            // 保持当前 yaw
            Ogre::Radian currentYaw = camNode->getOrientation().getYaw();

            // 设置新朝向
            Ogre::Quaternion newOri =
                Ogre::Quaternion(currentYaw, Ogre::Vector3::UNIT_Y) *
                Ogre::Quaternion(targetPitch, Ogre::Vector3::UNIT_X);

            camNode->setOrientation(newOri);
        }
    };
}; // end of namespace