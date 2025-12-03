
#pragma once
#include <vector>
#include <Ogre.h>
#include <OgreColourValue.h>
#include "fg/State.h"
#include "fg/CoreMod.h"
#include "fg/util/CostMap.h"
#include "fg/Cell.h"
#include "fg/util/CostMap.h"
#include "fg/MeshBuild.h"
#include "fg/VarBag.h"
#include "fg/Property.h"
#include "fg/core/ManualState.h"

namespace fog
{
    using namespace Ogre;

    typedef Property::Ref<bool> boolRef;

    //
    class TilesState : public ManualState
    {
    public:
    public:
        TilesState() : ManualState()
        {
        }
        void init() override
        {
            ManualState::init();
        }
        void rebuildMesh() override
        {
            obj->clear();
            obj->begin(material, Ogre::RenderOperation::OT_TRIANGLE_LIST);
            int baseIdx = obj->getCurrentVertexCount();
            Cell::Center *cc = Context<Cell::Center>::get();

            int w = Context<Cell::Center>::get()->getWidth();
            int h = Context<Cell::Center>::get()->getHeight();

            for (int x = 0; x < w; x++)
            {
                for (int y = 0; y < h; y++)
                {
                    Cell::Instance a = cc->getCell(x, y);
                    Vector3 pA = a.getOrigin3D();
                    obj->position(pA);
                    obj->colour(ColourValue::Blue);
                    obj->normal(Vector3(0, 1, 0));
                }
            }

            for (int x = 0; x < w - 1; x++)
            {
                for (int y = 0; y < h - 1; y++)
                {

                    int a = baseIdx + x * w + y;
                    int b = baseIdx + (x + 1) * w + y;
                    int c = baseIdx + (x + 1) * w + (y + 1);
                    int d = baseIdx + x * w + (y + 1);

                    //
                    obj->triangle(a, b, c);
                    obj->triangle(a, c, d);
                }
            }

            obj->end();
        }
    };

}; // end of namespace