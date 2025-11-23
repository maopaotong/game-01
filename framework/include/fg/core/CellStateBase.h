
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/State.h"
#include "fg/Core.h"
#include "fg/util/CostMap.h"
#include "fg/Cell.h"
#include "fg/Global.h"
#include "fg/MaterialNames.h"
#include "fg/MeshBuild.h"
#define DEFAULT_CELL_HIGH_OFFSET .08f

namespace fog
{
    using namespace Ogre;

    //
    class CellStateBase : public State
    {

    protected:
        Ogre::ManualObject *obj;
        Ogre::SceneNode *node;
        std::string material = MaterialNames::materialNameInUse;

    public:
        CellStateBase()
        {

            Ogre::SceneManager *sceneMgr = Context<Core *>::get()->getSceneManager();
            obj = sceneMgr->createManualObject();
            node = sceneMgr->getRootSceneNode()->createChildSceneNode();
            node->attachObject(obj);
            //
        }

        virtual void init() override
        {
            rebuildMesh();
        }

        virtual void rebuildMesh() = 0;
    };

}; // end of namespace