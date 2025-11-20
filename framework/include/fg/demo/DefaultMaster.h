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

namespace fog
{
    class DefaultActor : public Actor
    {
        std::vector<Task *> tasks;

    public:
        CellKey getDestinationCell()
        {
        }

        bool tryTakeTask(Task *t)
        {
            tasks.push_back(t);
            
            // waiting the player to make decision.
            return true;
        }

    };
    class DefaultMaster : public Master
    {
        Actor *defaultActor = nullptr;
        std::vector<Actor *> actors;
        std::unordered_map<long, std::unique_ptr<Task>> allTasks;
        std::unordered_set<long> tasksToBeAssigned;
        std::unordered_set<long> tasksToBeChecked;
        long nextId = 0;

    public:
        DefaultMaster()
        {
        }

        void add(Task *task) override
        {
            allTasks[nextId] = std::unique_ptr<Task>(task);
            tasksToBeAssigned.insert(nextId);
            nextId++;
        }

        bool step(long time) override
        {

            for (auto tid : tasksToBeChecked)
            {
                auto it = allTasks.find(tid);
                Task *task = it->second.get();
                if (task->isDone() || task->step(time))
                {
                    allTasks.erase(it);
                }
            }

            for (auto tid : tasksToBeAssigned)
            {

                Task *task = allTasks[tid].get();
                Actor *actor = nullptr;
                for (int j = 0; j < actors.size(); j++)
                {
                    Actor *actorJ = actors[j];
                    if (actorJ->tryTakeTask(task))
                    {
                        actor = actorJ;
                        break;
                    }
                }

                if (!actor)
                {
                    if (defaultActor->tryTakeTask(task))
                    {
                        actor = defaultActor;
                    }
                }
                if (!actor)
                { // no actor to take the task.
                    throw std::runtime_error("no actor to take the task.");
                }
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