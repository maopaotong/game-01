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

    public:
        Building(BuildingType type) : ManualState(), type(type)
        {
        }

        virtual ~Building()
        {
        }

        virtual void init() override
        {
            ManualState::init();
            
        }
        

        void rebuildMesh() override
        {
            MeshBuild::SpiderNet buildMesh(obj);
            buildMesh.begin(this->material);
            //

            Node2D * node2D = Context<Node2D>::get();
            auto position=[node2D](Vector2 &pointOnLayer){
                float scale = node2D->getScale();
                 Vector2 pointIn2D = pointOnLayer * scale;
                 Vector3 positionIn3D = node2D->plane->to3DInPlane(pointIn2D);
                 return positionIn3D;
            };

            buildMesh(position, ColourValue::Green);
            buildMesh.end();
        }
    };
}; // end of namespace
