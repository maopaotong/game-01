#pragma once
#include "State.h"
#include "util/Polygon2.h"
#include "Ground.h"

namespace fog
{
    class WorldState : public State
    {
    protected:
        Ground *ground;

    public:
        WorldState(Ground *shape)
        {
            this->ground = shape;
        }

        virtual void init() override = 0;

        Ground *getGround()
        {
            return ground;
        }
    };
};