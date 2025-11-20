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

        template <typename F>
        void forEachTask(F &&func)
        {
            std::unordered_map<long, Task *> map;
            getAllTask(map);
            for (auto it = map.begin(); it != map.end(); it++)
            {
                func(it->first, it->second);
            }
        }

        virtual void getAllTask(std::unordered_map<long, Task *> &map)
        {
        }
    };

}; //