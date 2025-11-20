#pragma once
#include <string>
#include <vector>
#include <typeindex>
#include <vector>
#include <memory>
#include <type_traits>
#include <functional>

#include "Task.h"

namespace fog
{

    class Master
    {
    public:
        virtual bool step(long time) = 0;
        virtual void add(Task *task) = 0;
    };

}; //