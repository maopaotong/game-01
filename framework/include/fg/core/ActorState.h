#pragma once

#include <Ogre.h>
#include "fg/PathFollow2.h"
#include "fg/util/CellUtil.h"
#include "fg/util/CostMap.h"
#include "fg/State.h"
#include "PathState.h"
#include "fg/Pickable.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/util/CollectionUtil.h"
#include "fg/Movable.h"
#include "fg/Actor.h"
using namespace Ogre;

class ActorState : public State, public Pickable, public Ogre::FrameListener, public Movable,public Actor
{

protected:
    bool active = false;

    PathFollow2 *pathFolow = nullptr;
    Ogre::Entity *entity;
    CostMap *costMap;
    PathState *pathState;

    PathFollow2MissionState *mission = nullptr;
    std::vector<std::string> aniNames = {"RunBase", "RunTop"};

public:
    ActorState(CostMap *costMap, Core *core) : State()
    {
        this->costMap = costMap;
        pathState = new PathState(costMap, core);

        this->setPickable(this);
        this->setFrameListener(this);
        this->setMovable(this);
    }

    void init(Core* core) {
        SceneManager *sMgr = core->getSceneManager();
        this->create(sMgr, this->entity, this->sceNode);
        this->setSceneNode(sceNode);
    }

    virtual void create(SceneManager *sMgr, Entity *&entity, SceneNode *&node) = 0;

    void setEntity(Ogre::Entity *entity)
    {
        this->entity = entity;
    }
    Ogre::Entity *getEntity()
    {
        return this->entity;
    }
    void setActive(bool active)
    {
        this->active = active;
    }

    bool isActive()
    {
        return this->active;
    }
    PathFollow2 *getPath()
    {
        return this->pathFolow;
    }
    void setPath(PathFollow2 *path)
    {
        this->pathFolow = path;
    }
    bool pickUp(MovableObject *actorMo) override
    {
        // cout << "ActorState::afterPick" << endl;

        SceneNode *node = actorMo->getParentSceneNode();
        const Vector3 &actorPosition = node->getPosition();
        std::cout << "actor.pos:" << actorPosition << "" << std::endl;
        CellKey cKey;
        bool hitActorCell = CellUtil::findCellByPoint(costMap, Ground::Transfer::to2D(actorPosition), cKey);
        ActorState *actor = this;
        if (hitActorCell)
        {
            bool active = this->isActive();
            if (!active)
            {
                actor->setActive(true);
            }
            else
            {
                actor->setActive(false);
                actor->setPath(nullptr);
                CellKey start;
                if (this->pathState->getStart(start))
                {
                    //    markStateControls[MarkType::ACTIVE]->mark(start, false);
                    this->pathState->clearPath();
                }
            }
        }
        return this->active;
    }

    bool setTargetCell(CellKey &cKey2) override
    {
        if (!this->isActive())
        {
            return false;
        }
        // check if this state's position on the target cell
        Vector3 aPos3 = this->sceNode->getPosition();
        float height = 0.0f;
        Vector2 aPos2 = Ground::Transfer::to2D(aPos3, height);
        CellKey aCellKey;
        bool hitCell = CellUtil::findCellByPoint(costMap, aPos2, aCellKey);
        if (hitCell)
        {
            std::vector<Vector2> pathByKey = costMap->findPath(aCellKey, cKey2);
            std::vector<Vector2> pathByPosition(pathByKey.size());
            CellUtil::translatePathToCellCenter(pathByKey, pathByPosition);
            PathFollow2 *path = new PathFollow2(aPos2, pathByPosition);
            this->setPath(path);
            pathState->setPath(pathByKey, aCellKey, cKey2);
            AnimationStateSet *anisSet = entity->getAllAnimationStates();

            // new child state.
            PathFollow2MissionState *missionState = new PathFollow2MissionState(path, anisSet, aniNames, height);
            // delete missionState;

            if (this->mission)
            {
                //int size = this->children->size();
                this->removeChild(this->mission);
                // std::cout << "children:size" << size << ",after remove child size:" << this->children->size() << std::endl;

                delete this->mission;
            }
            this->addChild(missionState);
            this->mission = missionState;
        }

        return true;
    }

    bool frameStarted(const FrameEvent &evt) override
    {
        std::function<void(State *)> func = [&evt](State *cState)
        {
            FrameListener *fl = cState->getFrameListener();
            if (fl)
            {
                fl->frameStarted(evt);
            }
        };
        this->forEachChild(func);
        return true;
    }
};