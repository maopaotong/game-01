#pragma once
#include <string>
#include <vector>
#include <typeindex>

#include <vector>
#include <memory>
#include <type_traits>
#include <functional>
#include <stack>
#include "Stairs.h"
#include "OgreFrameListener.h"
namespace fog
{

    using namespace Ogre;
    class Tasks
    {

    public:
        class Target
        {
        protected:
            long typeMask;

        public:
            long getTypeMask()
            {
                return typeMask;
            }
        };

        class Owner;

        class Task : public Stairs
        {
        protected:
            Owner *owner;
            Target *target;

        public:
            Task(Target *target, Owner *owner) : owner(owner), target(target)
            {
            }
            virtual bool step(float time) override = 0;
            virtual void destroy() = 0;
        };

        class Owner
        {

        protected:
            std::stack<std::unique_ptr<Task>> tasks;

            long targetTypeMask;

            long popCounter = 0;

        public:
            Owner(long targetTypeMask) : targetTypeMask(targetTypeMask)
            {
            }
            virtual ~Owner()
            {
            }

            long stackSize()
            {
                return tasks.size();
            }

            long getTargetTypeMask()
            {
                return targetTypeMask;
            }

            virtual bool tryTakeTarget(Target *target) = 0;

            void push(Task *task)
            {
                this->tasks.push(std::unique_ptr<Task>(task));
            }
            void pop()
            {

                this->tasks.pop();
                this->popCounter++;
            }

            long getPopCounter()
            {
                return popCounter;
            }
            Task *top()
            {
                return tasks.top().get();
            }
        };
        class Runner : public FrameListener
        {

            virtual bool frameStarted(const FrameEvent &evt)
            {

                return true;
            }
        };
    };

}; //