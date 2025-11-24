#pragma once
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#include "fg/CellInstanceManager.h"
#include "fg/core/MoveToCellTask.h"
namespace fog
{

    class MovingStateManager : public State, public Stairs
    {
        std::vector<MoveToCellTask *> tasks;

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
            Context<State *>::get()->forEach([this, &cKey2](State *state)
                                             {
                                                 if (state->isActive())
                                                 {
                                                     this->addTask(new MoveToCellTask(state, cKey2));
                                                 }
                                                 return true; //
                                             });
        }
        void addTask(MoveToCellTask *task)
        {
            this->tasks.push_back(task);
        }

        GOON step(float time) override
        {
            CellInstanceManager *cisManager = Context<CellInstanceManager *>::get();
            for (MoveToCellTask *task : this->tasks)
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