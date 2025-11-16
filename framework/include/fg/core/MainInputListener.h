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
#include "fg/IWorld.h"
#include "fg/Ground.h"
#include "fg/Core.h"

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
    IWorld *wsc;

    Core *core;
    float *cameraTopDistanceVptr;
    float *cameraHighMinVptr;
    float *cameraHighMaxVptr;
    float *cameraRollSpeedVptr;
    Global *global;

public:
    MainInputListener(IWorld *wsc,
                      Core *core)
    {
        this->core = core;
        this->wsc = wsc;
        global = core->getGlobal();
        this->cameraTopDistanceVptr = global->Var<float>::Bag::createBindVptr(".viewportTopDistance", DEFAULT_CAMERA_TOP_DISTANCE, 0.0f, DEFAULT_CAMERA_TOP_DISTANCE * 3); //
        this->cameraHighMinVptr = global->Var<float>::Bag::createBindVptr(".cameraHighMin", DEFAULT_CAMERA_HIGH_MIN, 0.0f, DEFAULT_CAMERA_HIGH_MIN * 3);                   //
        this->cameraHighMaxVptr = global->Var<float>::Bag::createBindVptr(".cameraHighMax", DEFAULT_CAMERA_HITH_MAX, 0.0f, DEFAULT_CAMERA_HITH_MAX * 3);                   //
        this->cameraRollSpeedVptr = global->Var<float>::Bag::createBindVptr(".cameraRollSpeed", DEFAULT_CAMERA_ROLL_SPEED, 0.0f, DEFAULT_CAMERA_ROLL_SPEED * 3);           //
    }

    bool mousePressed(const MouseButtonEvent &evt) override
    {

        if (evt.button == ButtonType::BUTTON_RIGHT)
        {

            setTargetByMouse(evt.x, evt.y);
        }

        return false;
    }

    void setTargetByMouse(int mx, int my)
    {
        // normalized (0,1)
        Viewport *viewport = core->getViewport();
        Camera *camera = core->getCamera();

        float ndcX = mx / (float)viewport->getActualWidth();
        float ndcY = my / (float)viewport->getActualHeight();

        Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

        Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0); // Y = 0
        auto hitGrd = ray.intersects(ground);
        std::cout << "ndc:(" << ndcX << "," << ndcY << ")" << "hit:" << hitGrd.first << std::endl;
        if (hitGrd.first)
        {
            Ogre::Vector3 pos = ray.getPoint(hitGrd.second);

            CellKey cKey;
            CostMap *costMap = this->wsc->getCostMap();

            // bool hitCell = CellUtil::findCellByPoint(costMap, Vector2(pos.x, pos.z), cKey);
            bool hitCell = CellUtil::findCellByPoint(costMap, Ground::Transfer::to2D(pos), cKey);

            if (hitCell)
            {
                void (*func)(State *, CellKey &) =
                    [](State *s, CellKey &cKey)
                {
                    Movable *mvb = s->getMovable();
                    if (mvb)
                    {
                        mvb->setTargetCell(cKey);
                    }
                };

                core->getRootState()->forEachChild<CellKey &>(
                    func,
                    cKey);
                //
            }
            // cout << "worldPoint(" << pickX << ",0," << pickZ << "),cellIdx:[" << cx << "," << cy << "]" << endl;
        }
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
        Camera *cam = core->getCamera();
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

        global->Var<Vector3>::Bag::setVar(".camera.position", posTarget);

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
