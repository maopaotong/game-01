
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/util/DrawerUtil.h"
#include "fg/State.h"
#include "fg/Core.h"
#include "fg/util/CostMap.h"
#include "fg/Cell.h"
#include "fg/Global.h"
#include "fg/MaterialNames.h"
#define DEFAULT_CELL_HIGH_OFFSET .08f

namespace fog
{
    using namespace Ogre;

    //
    class CellStateBase : public State
    {
    public:
    private:
        Ogre::ManualObject *obj;
        Ogre::SceneNode *node;
        float highOffset = DEFAULT_CELL_HIGH_OFFSET;
        std::string material = MaterialNames::materialNameInUse;

    public:
        CellStateBase(Core *core)
        {
            Ogre::SceneManager *sceneMgr = core->getSceneManager();
            obj = sceneMgr->createManualObject();
            node = sceneMgr->getRootSceneNode()->createChildSceneNode();
            node->attachObject(obj);
            node->setPosition(0, highOffset, 0);
            //
        }
        void init() override
        {
            this->rebuildMesh();
        }

        void rebuildMesh()
        {
            obj->clear();

            // Begin the manual object
            obj->begin(this->material, Ogre::RenderOperation::OT_TRIANGLE_LIST);
            Cell::Center *cc = Global::Context<Cell::Center *>::get();

            cc->forEachCell([this](Cell::Instance &cell)
                            { this->buildInstanceMesh(obj, cell); });

            // End the manual object
            obj->end();
        }

        virtual void buildInstanceMesh(ManualObject *obj, Cell::Instance &cell)
        {
            Ogre::ColourValue color = ColourValue::White;
            cell.buildMesh(obj, color);
        }
    };

}; // end of namespace