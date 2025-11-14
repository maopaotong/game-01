#pragma once

#include <vector>
#include <Ogre.h>
#include "util/Polyline2.h"

using namespace Ogre;

class PathFollow2
{
    std::vector<Ogre::Vector2> path;
    float speedFactor = 30.0f;
    float &speed;
    int next = 1; // ignore the first one.
    Vector2 position;

public:
    PathFollow2(Vector2 position, std::vector<Ogre::Vector2> path, float &speed) : speed(speed)
    {
        this->position = position;
        this->path = path;
    }

    float *getSpeedPtr()
    {
        return &speed;
    }

    bool move(float timeEscape, Vector2 &currentPos, Vector2 &direction)
    {
        bool rt = false;
        while (next < path.size())
        {
            Vector2 nextPos = path[next];   // next position
            direction = nextPos - position; // direction

            float distance = direction.length();
            if (distance < 0.01f)
            {
                next++;
                continue;
            }
            // found the right position
            direction.normalise();
            float move = speed * speedFactor * timeEscape;
            if (move > distance)
            {
                move = distance;
            }

            position += direction * move;
            currentPos = this->position;
            rt = true;
            break;
        }
        return rt;
    }
};