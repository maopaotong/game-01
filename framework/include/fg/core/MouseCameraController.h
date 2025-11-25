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

namespace fog
{
    using namespace OgreBites;
    using namespace Ogre;
    // === Custom hash function ===
    //
    // === Input handler for closing application ===
    class MainInputListener : public OgreBites::InputListener
    {
        static constexpr float DEFAULT_CAMERA_TOP_DISTANCE = 2 * 1000.0f;
        static constexpr float DEFAULT_CAMERA_HIGH_MIN = 100.0f;
        static constexpr float DEFAULT_CAMERA_HITH_MAX = 1000.0f;
        static constexpr float DEFAULT_CAMERA_ROLL_SPEED = (DEFAULT_CAMERA_HITH_MAX - DEFAULT_CAMERA_HIGH_MIN) / 10.0f;

    private:
        float *cameraTopDistanceVptr;
        float *cameraHighMinVptr;
        float *cameraHighMaxVptr;
        float *cameraRollSpeedVptr;
        CostMap *costMap;

    public:
        MainInputListener(CostMap *costMap) : costMap(costMap)
        {
            this->cameraTopDistanceVptr = Context<Var<float>::Bag>::get()->createBindVptr(".viewportTopDistance", DEFAULT_CAMERA_TOP_DISTANCE, 0.0f, DEFAULT_CAMERA_TOP_DISTANCE * 3); //
            this->cameraHighMinVptr = Context<Var<float>::Bag>::get()->createBindVptr(".cameraHighMin", DEFAULT_CAMERA_HIGH_MIN, 0.0f, DEFAULT_CAMERA_HIGH_MIN * 3);                   //
            this->cameraHighMaxVptr = Context<Var<float>::Bag>::get()->createBindVptr(".cameraHighMax", DEFAULT_CAMERA_HITH_MAX, 0.0f, DEFAULT_CAMERA_HITH_MAX * 3);                   //
            this->cameraRollSpeedVptr = Context<Var<float>::Bag>::get()->createBindVptr(".cameraRollSpeed", DEFAULT_CAMERA_ROLL_SPEED, 0.0f, DEFAULT_CAMERA_ROLL_SPEED * 3);           //
        }
       

        bool mouseReleased(const MouseButtonEvent &evt) override
        {
            if (evt.button == ButtonType::BUTTON_LEFT)
            {
            }
            return false;
        }

        bool mouseWheelRolled(const MouseWheelEvent &evt) override
        {
            Camera *cam = Context<CoreMod>::get()->getCamera();
            Ogre::SceneNode *node = cam->getParentSceneNode();
            Vector3 translate = Ogre::Vector3::NEGATIVE_UNIT_Y * evt.y * *cameraRollSpeedVptr;
            Vector3 posTarget = node->getPosition() + translate;
            if (posTarget.y < *this->cameraHighMinVptr)
            {
                posTarget.y = *this->cameraHighMinVptr;
            }
            if (posTarget.y > *this->cameraHighMaxVptr)
            {
                posTarget.y = *this->cameraHighMaxVptr;
            }

            node->setPosition(posTarget);

            Context<Var<Vector3>::Bag>::get()->setVar(".camera.position", posTarget);

            alignHorizonToTop(node, cam, *this->cameraTopDistanceVptr);
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