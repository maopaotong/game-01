#pragma once
#include <string>
#include <vector>
#include <typeindex>
#include <vector>
#include <memory>
#include <type_traits>
#include <functional>

#include "fg/Task.h"
#include "fg/Master.h"
#include "fg/Actor.h"
#include "fg/State.h"

namespace fog
{

    class DefaultMaster : public Master, public FrameListener
    {
        Actor *defaultActor = nullptr;
        std::unordered_map<long, std::unique_ptr<Task>> allTasks;
        std::unordered_set<long> tasksToBeAssigned;
        std::unordered_set<long> tasksToBeChecked;
        long nextId = 0;
        State *root;

    public:
        DefaultMaster(State *root) : root(root)
        {
        }

        void add(Task *task) override
        {
            allTasks[nextId] = std::unique_ptr<Task>(task);
            tasksToBeAssigned.insert(nextId);
            nextId++;
        }

        Task::Owner *tryCreateOwner(std::type_index ownerType)
        {

            Task::Owner *ret = nullptr;

            root->forEachChild([&ret, ownerType](State *state)
                               {
                
                std::type_index otype = state->getTaskOwnerType();
                if(ownerType == otype){
                    ret = state->createTaskOwner();        
                    return true;
                }
                return false; });

            return ret;
        }
        bool frameStarted(const FrameEvent &evt)
        {
            this->step(evt.timeSinceLastFrame);
            return true;
        }
        bool frameRenderingQueued(const FrameEvent &evt)
        {
            (void)evt;
            return true;
        }
        bool step(long time) override
        {

            std::unordered_set<long> doneSet;
            for (auto tid : tasksToBeChecked)
            {
                auto it = allTasks.find(tid);
                if (it->first)
                {
                    Task *task = it->second.get();
                    if (task->isDone() || task->step(time))
                    {
                        doneSet.insert(tid);
                    }
                }
            }
            for (auto tid : doneSet)
            {

                allTasks.erase(tid);
                doneSet.erase(tid);
            }

            std::unordered_set<long> processed;
            for (auto tid : tasksToBeAssigned)
            {

                Task *task = allTasks[tid].get();
                Task::Owner *owner = this->tryCreateOwner(task->getOwnerType());
                if (owner)
                {

                    task->start(owner);
                    processed.insert(tid);
                }
                else
                {
                    // ignore task.
                }
            }

            for (auto tid : processed)
            {
                tasksToBeAssigned.erase(tid);
                tasksToBeChecked.insert(tid);
            }

            if (allTasks.empty())
            {
                return true; // all task is done, game is over.
            }
            return false;
        }
    };

}; //