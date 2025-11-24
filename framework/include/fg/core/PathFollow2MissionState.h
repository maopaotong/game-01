#pragma once
#include <Ogre.h>
#include <OgreFrameListener.h>
#include <OgreAnimationState.h>
#include <vector>
#include "fg/PathFollow2.h"

namespace fog
{
    using Vector3Ref = Property::Ref<Vector3>;
    /**
     * Move a node to a destination.
     */
    class PathFollow2MissionState : public State, public Stairs
    {
        PathFollow2 path;

        AnimationStateSet *aniSet;

        Vector3 offset;

        float animateTimeSpeedFactor;
        //Vector3Ref actorPosition;
        State *state;
        bool done;

    public:
        PathFollow2MissionState(State *state, PathFollow2 path,                     //
                                AnimationStateSet *aniSet,                          //
                                std::vector<std::string> &aniNames, float aniSpeed, //
                                float heightOffset = 0.0f)                          //
            : state(state),                                                         //
              path(path),                                                           //
              aniSet(aniSet),                                                       //
              animateTimeSpeedFactor(aniSpeed),                                     //
              done(false)                                                           //
        {
            this->aniSet = aniSet;
            this->offset = Vector3(0, heightOffset, 0);
            for (std::string name : aniNames)
            {
                AnimationState *as = this->aniSet->getAnimationState(name);
                as->setEnabled(true);
                as->setLoop(true);
                as->setWeight(1.0f);
            }
            //
            // this->setFrameListener(this);
        }
        void init() override
        {
           // actorPosition = this->getProperty<Vector3>("actor1"
                                     //                  ".actor.position");
        }

        PathFollow2 &getPath()
        {
            return this->path;
        }

        Vector3 to3D(Vector2 pointIn2D)
        {

            return Context<Node2D *>::get()->to3D(pointIn2D) + this->offset;
        }

        // bool frameStarted(const Ogre::FrameEvent &evt) override
        bool step(float timeSinceLastFrame)
        {
            if (this->done)
            {
                return false;
            }

            PathFollow2 &pathFollow = this->getPath();

            Vector2 currentPos2D;
            Vector2 direction2D;
            if (!pathFollow.move(timeSinceLastFrame, currentPos2D, direction2D))
            {
                this->done = true;
                return false;
            }

            //
            SceneNode *target = this->state->findSceneNode();
            Vector3 prevPos = target->getPosition();

            float terH = 0.0f; // Context<Terrains *>::get()->getHeightAtPosition(currentPos2D); // TODO in a common place to translate all .

            Vector3 currentPos = this->to3D(currentPos2D);
            // position
            target->translate(currentPos - prevPos); // new position
            // high

            // animation
            AnimationStateIterator it = this->aniSet->getAnimationStateIterator();
            while (it.hasMoreElements())
            {
                AnimationState *as = it.getNext();
                float aniTimeFactor = this->animateTimeSpeedFactor * (path.getSpeed());
                as->addTime(timeSinceLastFrame * aniTimeFactor);
            }

            Vector3 d3 = Vector3(direction2D.x, 0, -direction2D.y);
            Quaternion orientation = Vector3::UNIT_Z.getRotationTo(d3);

            target->setOrientation(orientation);

            //actorPosition = target->getPosition();
            // pNode->lookAt();

            // pNode->setOrientation(Quaternion(Degree(90), Vector3::UNIT_Y));
            //   update direction
            //

            return true;
        }
    };
}; // end of namespace