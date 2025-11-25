
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/util/CostMap.h"
#include "fg/Cell.h"
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
        
        std::string material = MaterialNames::materialNameInUse;

    public:
        CellStateBase()
        {

            Ogre::SceneManager *sceneMgr = Context<CoreMod>::get()->getSceneManager();
            obj = sceneMgr->createManualObject();
            this->sceNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
            sceNode->attachObject(obj);
            //
        }
        virtual ~CellStateBase()
        {
            Ogre::SceneManager *sceneMgr = Context<CoreMod>::get()->getSceneManager();
            sceneMgr->getRootSceneNode()->removeChild(this->sceNode);
            sceneMgr->destroyManualObject(this->obj);
        }

        virtual void init() override
        {
            rebuildMesh();
        }

        virtual void rebuildMesh() = 0;
    };

}; // end of namespace