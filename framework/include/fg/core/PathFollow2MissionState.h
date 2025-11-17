#pragma
#include <Ogre.h>
#include <OgreFrameListener.h>
#include <OgreAnimationState.h>
#include <vector>
#include "fg/core/MissionState.h"
#include "fg/PathFollow2.h"

namespace fog
{
    /**
     * Move a node to a destination.
     */
    class PathFollow2MissionState : public MissionState, public FrameListener
    {
        PathFollow2 *path;

        AnimationStateSet *aniSet;
        float heightOffset = 0.0f;

        float animateTimeSpeedFactor;

    public:
        PathFollow2MissionState(Global *global, PathFollow2 *path, AnimationStateSet *aniSet, std::vector<std::string> &aniNames, float aniSpeed, float heightOffset = 0.0f) : animateTimeSpeedFactor(aniSpeed)
        {

            this->path = path;
            this->aniSet = aniSet;
            this->heightOffset = heightOffset;
            for (std::string name : aniNames)
            {
                AnimationState *as = this->aniSet->getAnimationState(name);
                as->setEnabled(true);
                as->setLoop(true);
                as->setWeight(1.0f);
            }
            //
            this->setFrameListener(this);
        }

        PathFollow2 *getPath()
        {
            return this->path;
        }

        Vector3 to3D(Vector2 pointIn2D)
        {

            return Global::Context<Node2D *>::get()->to3D(pointIn2D, this->heightOffset);
        }

        bool frameStarted(const Ogre::FrameEvent &evt) override
        {
            SceneNode *pNode = this->findSceneNode(); // the parent node to operate

            if (pNode) // if the parent has no scene node attached,ignore the update operation.
            {
                PathFollow2 *pathFollow = this->getPath();

                Vector2 currentPos2D;
                Vector2 direction2D;
                if (pathFollow->move(evt.timeSinceLastFrame, currentPos2D, direction2D))
                {

                    //
                    Vector3 prevPos = pNode->getPosition();

                    float terH = 0.0f; // Global::Context<Terrains *>::get()->getHeightAtPosition(currentPos2D); // TODO in a common place to translate all .

                    // Vector3 currentPos = Ground::Transfer::to3D(currentPos2D, Global::getTerrainHeightAtPositionWithOffset, heightOffset); //
                    Vector3 currentPos = this->to3D(currentPos2D);
                    // position
                    pNode->translate(currentPos - prevPos); // new position
                    // high

                    // animation
                    AnimationStateIterator it = this->aniSet->getAnimationStateIterator();
                    while (it.hasMoreElements())
                    {
                        AnimationState *as = it.getNext();
                        float aniTimeFactor = this->animateTimeSpeedFactor * (path->getSpeed());
                        as->addTime(evt.timeSinceLastFrame * aniTimeFactor);
                    }

                    Quaternion orientation = Ground::getRotationTo(direction2D);
                    pNode->setOrientation(orientation);

                    // pNode->lookAt();

                    // pNode->setOrientation(Quaternion(Degree(90), Vector3::UNIT_Y));
                    //   update direction
                    //
                }
                else
                {
                    this->setDone(true);
                }
            }
            else
            {
                // todo add warning here if no scene node found.
            }
            return true;
        }
    };
}; // end of namespace