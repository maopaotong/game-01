#pragma once
#include "fg/defines.h"

#include <typeindex>

#include <memory>
#include <type_traits>
#include <utility>
#include <Ogre.h>
#include <fg/Cell.h>
#include <fg/Context.h>
#include "OgreSceneNode.h"
#include "fg/util/CostMap.h"
#include "fg/PathFollow2.h"
#include "fg/core/PathState.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/Tasks.h"
#include "fg/Cell.h"
#include "fg/HoverCellState.h"

namespace fog
{
    using BaseTask = Tasks::Task;
    class PathFindTask : public BaseTask
    {
        float actorHighOffset;

    protected:
        CellKey cKey2;
        CellKey preCKey2;
        State *state;
        PathState *pathState = nullptr;

    public:
        PathFindTask(State *state) : state(state)
        {
            std::cout << "PathFindTask created." << std::endl;
        }
        virtual ~PathFindTask()
        {
              std::cout << "~PathFindTask destroyed." << std::endl;
        }

        bool pause() override
        {
            return true;
        }

        bool cancel() override
        {

            return false;
        }

        bool resume() override
        {
            return true;
        }

        GOON step(float time) override
        {
            CellKey cKey2 = Context<HoverCellState *>::get()->getCellKey();

            if (this->cKey2 == cKey2)
            { // do nothing.
                return true;//GOON
            }

            this->preCKey2 = this->cKey2;
            this->cKey2 = cKey2;

            Cell::Instance cell1;
            Context<Cell::Center *>::get()->findCellByWorldPosition(state->getSceneNode()->getPosition(), cell1);

            CellKey cKey1 = cell1.cKey;

            std::vector<Vector2> pathByPoint2DNom = Context<CostMap *>::get()->findPath(cKey1, cKey2);

            PathState *pathState2 = new PathState();
            pathState2->init();
            pathState2->setPath(pathByPoint2DNom, cKey1, cKey2);
            if (pathState)
            {
                this->removeChild(pathState);
            }
            pathState = pathState2;
            this->addChild(pathState);

            return true;
        }

    }; //
};