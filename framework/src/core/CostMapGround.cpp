#pragma once

#include "fg/core/CostMapGround.h"
namespace fog
{
CostMapGround::CostMapGround(CostMap *costMap)
{
    this->costMap = costMap;

    int width = costMap->getWidth();
    int height = costMap->getHeight();
    Vector2 offset = CellUtil::offset(costMap);

    float rX = width * CostMap::hexSize * 2;
    float rZ = height * CostMap::hexSize * 2;

    polygon.add(Vector2(0, 0));
    polygon.add(Vector2(0, rZ));
    polygon.add(Vector2(rX, rZ));
    polygon.add(Vector2(rX, 0));
    polygon += offset;
}
CostMap *CostMapGround::getCostMap()
{
    return costMap;
}
bool CostMapGround::isPointInside(float x, float y)
{
    return polygon.isPointInPolygon(x, y);
}
bool CostMapGround::isPointInside(Vector2 &p)
{
    return polygon.isPointInPolygon(p.x, p.y);
}
};