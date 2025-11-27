#pragma once

#include <Ogre.h>
#include "fg/PathFollow2.h"
#include "fg/util/CellUtil.h"
#include "fg/util/CostMap.h"
#include "fg/State.h"
#include "fg/Pickable.h"
#include "fg/core/PathFollow2MissionState.h"
#include "fg/util/CollectionUtil.h"
#include "fg/core/ManualState.h"
#include "fg/MeshBuild.h"
namespace fog
{
    using namespace Ogre;
    using Vector3Ref = Property::Ref<Vector3>;

    class Building : public ManualState
    {

    protected:
        BuildingType type;
        CellKey cKey;
        float offsetY = 10.0f;

    public:
        Building(BuildingType type, CellKey cKey) : ManualState(), cKey(cKey), type(type)
        {
        }

        virtual ~Building()
        {
        }

        virtual void init() override
        {
            ManualState::init();
        }
        void setCellKey(CellKey ckey)
        {
            this->cKey = ckey;
            rebuildMesh();
        }        

        void rebuildMesh() override
        {
            MeshBuild::SpiderNet buildMesh(obj);
            buildMesh.begin(this->material);
            Cell::Instance ci = Context<Cell::Center>::get()->getCell(CellKey(0, 0));
            buildMesh(ci, ColourValue::Green);
            buildMesh.end();
        }
    };
}; // end of namespace
