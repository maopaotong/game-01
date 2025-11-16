#pragma once

#include "fg/Ground.h"
#include "fg/util/CostMap.h"
#include "fg/util/Polygon2.h"

namespace fog{
class CostMapGround : public Ground
{
private:
    CostMap *costMap;
    Polygon2 polygon;

public:
    CostMapGround(CostMap *costMap);
    CostMap *getCostMap();
    bool isPointInside(float x, float z) override;
    bool isPointInside(Vector2 &p) override;
};

};//end of namespace