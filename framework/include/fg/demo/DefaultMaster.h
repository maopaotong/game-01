#pragma once
#include <string>
#include <vector>
#include <typeindex>
#include <vector>
#include <memory>
#include <type_traits>
#include <functional>

#include "fg/Tasks.h"
#include "fg/Master.h"
#include "fg/Actor.h"
#include "fg/State.h"

namespace fog
{
/*
*/
    class DefaultMaster : public Master, public FrameListener
    {
        Actor *defaultActor = nullptr;
        std::unordered_map<long, std::unique_ptr<Task>> allTasks;
        std::unordered_set<long> tasksToBeAssigned;
        std::unordered_set<long> tasksToBeChecked;
        long nextId = 1;
        State *root;

    public:
        DefaultMaster(State *root) : root(root)
        {
        }

        void getAllTask(std::unordered_map<long, Task *> &map) override
        {
            for (auto it = this->allTasks.begin(); it != allTasks.end(); it++)
            {
                map[it->first] = it->second.get();
            }
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
            
            for (auto it = tasksToBeChecked.begin(); it != tasksToBeChecked.end();)
            {

                long tid = *it;

                auto it2 = allTasks.find(tid);
                bool ereased = false;
                if (it2->first)
                {
                    Task *task = it2->second.get();
                    if (task->isDone() || task->step(time))
                    {
                        allTasks[tid]->destroy();
                        allTasks.erase(tid);
                        it = tasksToBeChecked.erase(it);
                        ereased = true;
                    }
                }
                if (!ereased)
                {
                    it++;
                }
            }

            for (auto it = tasksToBeAssigned.begin(); it != tasksToBeAssigned.end();)
            {

                long tid = *it;
                Task *task = allTasks[tid].get();
                Task::Owner *owner = this->tryCreateOwner(task->getOwnerType());
                if (owner)
                {
                    task->start(owner);
                    tasksToBeChecked.insert(tid);
                    it = tasksToBeAssigned.erase(it);
                }
                else
                {
                    it++;
                }
            }

            //for each idle state, ask the manager for a task.
            root->forEachChild([time](State* state){
                Task* task = state->getTask();
                if(task){
                    task->step(time);
                    if(task->isDone()){
                        state->pop();
                    }
                }
                return false;
            });

            if (allTasks.empty())
            {
                return true; // all task is done, game is over.
            }
            return false;
        }
    };

}; //