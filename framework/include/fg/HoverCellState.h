#pragma once
#include "fg/defines.h"
#include "fg/Cell.h"
#include "fg/Context.h"
#include "fg/State.h"
#include "fg/CoreMod.h"

namespace fog
{
    class HoverCellState : public State
    {
        CellKey preCKey;
        CellKey cKey;

    public:
        HoverCellState()
        {
        }
        virtual ~HoverCellState()
        {
        }

        CellKey getCellKey()
        {
            return cKey;
        }

        void onMouseMoving(int x, int y)
        {
            Viewport *viewport = Context<CoreMod *>::get()->getViewport();
            Camera *camera = Context<CoreMod *>::get()->getCamera();

            float ndcX = x / (float)viewport->getActualWidth();
            float ndcY = y / (float)viewport->getActualHeight();

            Ogre::Ray ray = camera->getCameraToViewportRay(ndcX, ndcY);

            Ogre::Plane ground(Ogre::Vector3::UNIT_Y, 0); // Y = 0
            auto hitGrd = ray.intersects(ground);

            Ogre::Vector3 pos2;
            if (hitGrd.first)
            {
                pos2 = ray.getPoint(hitGrd.second);
            }
            else
            {
                return;
            }

            Cell::Instance cell2;
            Context<Cell::Center *>::get()->findCellByWorldPosition(pos2, cell2);
            if (cKey != cell2.cKey)
            {
                // hover changed.
                preCKey = cKey;
                cKey = cell2.cKey;
            }
        }
    };
};