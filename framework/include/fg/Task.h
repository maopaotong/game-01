#pragma once
#include <string>
#include <vector>
#include <typeindex>

#include <vector>
#include <memory>
#include <type_traits>
#include <functional>

namespace fog
{

    class Task
    {
        std::type_index taskType;
        std::type_index ownerType;

    public:
        class Owner
        {
        public:
            virtual ~Owner() {}
        };

        template <typename T, typename O>
        explicit Task(std::in_place_type_t<T>, std::in_place_type_t<O>) : taskType(typeid(T)), ownerType(typeid(O))
        {
        }

        std::type_index getTaskType()
        {
            return taskType;
        }

        std::type_index getOwnerType()
        {
            return ownerType;
        }

        virtual void start(Owner *owner) = 0;

        virtual bool isDone() = 0;
        virtual bool step(long time) = 0;
    };

}; //