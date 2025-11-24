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
    class State;
    class Tasks
    {

    public:
        class Task : public Stairs
        {
        protected:
            List<UniquePtr<State>> states; // child states.
            void addChild(State *state)
            {
                this->states.push_back(UniquePtr<State>(state));
            }
            void removeChild(State *state)
            {
                this->states.erase(
                    std::remove_if(this->states.begin(), this->states.end(), [state](const UniquePtr<State> &s)
                                   { return state == s.get(); }),
                    this->states.end());
            }

        public:
            Task()
            {
            }
            virtual ~Task()
            {
            }

            virtual bool step(float time) override = 0;
            /**
             * if the task is pauseable , new task will push to the top of the stack.
             * if not pausable , the new add task will be append to a queue.
             * When the top task pop up, the tasks in the quque will be scheduled first.
             * So this option can be used to control the priority of tasks.
             *
             */
            virtual bool pause() = 0;
            virtual bool resume() = 0;
            virtual bool cancel() = 0;
        };

        class Slot
        {

        protected:
            std::stack<std::unique_ptr<Task>> stack;

            long popCounter = 0;

            void doPush(Task *task)
            {
                this->stack.push(std::unique_ptr<Task>(task));
            }

            bool tryPush(Task *task)
            {
                if (this->stack.empty() || this->top()->pause())
                {
                    this->doPush(task);

                    return true;
                }

                return false;
            }

        public:
            long stackSize()
            {
                return stack.size();
            }

            bool empty()
            {
                return stack.empty();
            }

            template <typename F>
            bool tryCancelAndPush(F &&newTask)
            {
                Task *preTask = this->tryGetTop();
                if (preTask)
                {
                    if (preTask->cancel())
                    {
                        this->pop();
                    }
                    else
                    {
                        return false;
                    }
                }

                Task *task = newTask();
                this->doPush(task);
                return true;
            }

            template <typename F>
            void push(F &&newTask)
            {                                
                Task *task = newTask();
                this->doPush(task);                
            }

            Task *tryGetTop()
            {
                if (this->stack.empty())
                {
                    return nullptr;
                }
                return stack.top().get();
            }

            Task *top()
            {
                return stack.top().get();
            }

            void pop()
            {
                this->stack.pop();
                this->popCounter++;
                Task *top = this->tryGetTop();
                if (top)
                {
                    top->resume();
                }
                // resume one from the top.
            }

            void done()
            {
                this->pop();
            }

            long getPopCounter()
            {
                return popCounter;
            }

            bool step(float time)
            {
                if (this->stack.empty())
                {
                    return true;
                }
                Tasks::Task *task = this->stack.top().get();

                bool goOn = task->step(time);
                if (!goOn)
                {
                    this->stack.pop();
                }
                return true;
            }
        };
        class Slots : public Stairs
        {
            List<UniquePtr<Slot>> slots;

        public:
            Slot *slot(int idx)
            {
                while (slots.size() <= idx)
                {
                    slots.push_back(UniquePtr<Slot>(new Slot()));
                }
                return slots.at(idx).get();
            };
            Slot *tryGetSlot(int idx)
            {
                if (idx < slots.size())
                {
                    return slots.at(idx).get();
                }
                return nullptr;
            }
            bool step(float time)
            {
                for (auto &slot : slots)
                {
                    slot->step(time);
                }
                return true;
            }
        };
    };

}; //