#pragma once
#include <string>
#include <vector>
#include <typeindex>
#include "Options.h"
namespace fog
{
    class Game
    {
    public:
        virtual void start(Options *options) {};
        virtual ~Game() {};
    };
};