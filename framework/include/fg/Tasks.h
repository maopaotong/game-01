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
            /**
             * is the step0
             * @return false: will be delete and no nother method is called .
             */
            virtual bool init() = 0;
            virtual bool step(float time) override = 0;
            /**
             * if the task is pauseable , new task will push to the top of the stack.
             * if not pausable , the new add task will be append to a queue.
             * When the top task pop up, the tasks in the quque will be scheduled first.
             * So this option can be used to control the priority of tasks.
             *
             */
            virtual bool pause() = 0;
            virtual void resume() = 0;
            virtual void destroy() = 0;
            virtual void wait() = 0;
        };

        class Owner
        {

        protected:
            std::stack<std::unique_ptr<Task>> stack;
            std::vector<Task *> queue;

            long targetTypeMask;

            long popCounter = 0;

            void append(Task *task)
            {
                queue.push_back(task);
                task->wait();
            }

            void doPush(Task *task)
            {
                this->stack.push(std::unique_ptr<Task>(task));
            }

            bool tryPush(Task *task)
            {
                if (this->top()->pause())
                {
                    this->doPush(task);
                    return true;
                }
                
                return false;
            }

        public:
            Owner(long targetTypeMask) : targetTypeMask(targetTypeMask)
            {
            }
            virtual ~Owner()
            {
            }

            long stackSize()
            {
                return stack.size();
            }

            long getTargetTypeMask()
            {
                return targetTypeMask;
            }

            virtual bool tryTakeTarget(Target *target) = 0;

            void push(Task *task)
            {

                if (!task->init())
                {
                    delete task;
                    return;
                }
                if (this->tryPush(task))
                {
                    return;
                }
                this->append(task);
            }

            Task *top()
            {
                return stack.top().get();
            }

            void pop()
            {
                this->stack.pop();
                this->popCounter++;

                for (auto it = this->queue.begin(); it != this->queue.end();)
                {
                    Task *task = *it;
                    if (this->tryPush(task))
                    {                        
                        it = queue.erase(it);
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }

                this->top()->resume();
            }

            void done()
            {
                this->pop();
            }

            long getPopCounter()
            {
                return popCounter;
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