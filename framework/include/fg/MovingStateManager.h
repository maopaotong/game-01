#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/CellInstanceManager.h"
#include "fg/core/MoveToCellTask.h"
#include "fg/MovableStateManager.h"
namespace fog
{

    class MovingStateManager : public State, public Stairs
    {
        std::vector<std::unique_ptr<MoveToCellTask>> tasks;

    public:
        MovingStateManager()
        {
        }
        virtual ~MovingStateManager()
        {
        }
        void init() override
        {
        }
        void movingActiveStateToCell(CellKey cKey2)
        {

            // add new tasks
            Context<MovableStateManager *>::get()-> //
                forEach([this, &cKey2](State *state)
                        {
                            if (state->isActive())
                            {
                                this->addTask(state, cKey2);
                            }
                            return true; //
                        });
        }
        void addTask(State *state, CellKey cKey2)
        {
            for (auto &it = this->tasks.begin(); it != tasks.end();)
            {
                MoveToCellTask *task = it->get();
                if (task->getState() == state)
                {
                    it = tasks.erase(it);
                }
                else
                {
                    it++;
                }
            }

            MoveToCellTask *task = new MoveToCellTask(state, cKey2);
            this->tasks.push_back(std::unique_ptr<MoveToCellTask>(task));
        }
        template <typename F>
        void forEachTask(F &&f)
        {
            for (auto &task : this->tasks)
            {
                if (!f(task.get()))
                {
                    break;
                }
            }
        }

        GOON step(float time) override
        {
            CellInstanceManager *cisManager = Context<CellInstanceManager *>::get();
            for (auto &task : this->tasks)
            {
                task->step(time);
                State *state = task->getState();
                CellInstanceState *currentCis = cisManager->getCellInstanceStateByPosition(state->getSceneNode()->getPosition());
                if (currentCis)
                {
                    if (currentCis->getCellKey() != state->getCellKey())
                    {
                        CellInstanceState *preCis = cisManager->getCellInstanceStateByCellKey(state->getCellKey());
                        if (preCis)
                        {
                            preCis->unsetColour();
                        }

                        state->setCellKey(currentCis->getCellKey());

                        currentCis->setColour(Ogre::ColourValue::Green);
                    }
                }
            }
            return true;
        }

    }; // end of class
}; // end of namespace