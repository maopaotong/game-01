#pragma once
#include <Ogre.h>
#include <OgreSceneManager.h>
#include "fg/defines.h"
#include "fg/State.h"
#include "fg/core/ActorState.h"
#include "fg/Core.h"
#include "fg/Terrains.h"

namespace fog
{
    using namespace Ogre;
    using Vector3Ref = Property::Ref<Vector3>;

    class ActorSelectControl : public CellStateBase, public Ogre::FrameListener
    {
        Cell::Instance cell;

        bool active = false;
        Vector3Ref actorPosition;
        Vector3 prePosition;

    public:
        ActorSelectControl(CostMap *costMap, Core *core) : CellStateBase(core)
        {
        }

        void init() override
        {
            Cell::Center *cc = Context<Cell::Center *>::get();
            cell = cc->getAnyCell();
            actorPosition = this->getProperty<Vector3>("actor1"".actor.position");
            //prePosition = actorPosition;
            CellStateBase::init();
        }

        void rebuildMesh() override
        {
            if (!this->parent)
            {
                return;
            }
            MeshBuild::SpiderNet buildMesh(obj);
            buildMesh.begin(this->material);
            if (parent->isActive())
            {
                buildMesh(cell, ColourValue::White); //
                this->active = true;
            }
            else
            {
                Cell::Center *cc = Context<Cell::Center *>::get();
                cell = cc->getAnyCell();
                this->active = false;
            }

            buildMesh.end();
        }

        bool frameStarted(const FrameEvent &evt) override
        {

            if (!this->active && !this->parent->isActive())
            {
                return true;
            }

            if (this->active && !this->parent->isActive())
            {
                this->rebuildMesh();
                return true;
            }

            if (this->parent->isActive())
            {

                Cell::Center *cc = Context<Cell::Center *>::get();
                Cell::Instance cell2;
                //Vector3 position = this->parent->getSceneNode()->getPosition();
                
                if (prePosition.distance(actorPosition) > CostMap::hexSize)
                {

                    if (cc->findCellByWorldPosiion(actorPosition, cell2))
                    {
                    }

                    if (this->cell.cKey != cell2.cKey)
                    {
                        this->cell = cell2;
                        this->rebuildMesh(); // todo move the medh build to input event stage, not in frame event?
                    }
                    prePosition = actorPosition;
                }
            }

            return true;
        }
    };
}; // end of namespace