#pragma once

#include "fg/Ground.h"
#include "fg/util/CostMap.h"
#include "fg/util/Polygon2.h"
class DemoGround : public Ground
{
private:
    CostMap *costMap;
    Polygon2 polygon;

public:
    DemoGround(CostMap *costMap);
    CostMap *getCostMap();
    bool isPointInside(float x, float z) override;
    bool isPointInside(Vector2 &p) override;
};