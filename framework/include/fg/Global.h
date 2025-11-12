
#pragma once
#include "Actor.h"

class Global
{
public:
    void setActiveActor(Actor *actor)
    {
        this->activeActor = actor;
    }
    Actor *getActiveActor()
    {
        return this->activeActor;
    }

private:
    Actor *activeActor = nullptr;
};