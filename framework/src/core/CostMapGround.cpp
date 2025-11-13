#pragma once

#include "fg/core/CostMapGround.h"

CostMapGround::CostMapGround(CostMap *costMap)
{
    this->costMap = costMap;

    int width = costMap->getWidth();
    int height = costMap->getHeight();
    float rX = width * CostMap::hexSize * 2;
    float rZ = height * CostMap::hexSize * 2;

    polygon.add(0, 0);
    polygon.add(0, rZ);
    polygon.add(rX, rZ);
    polygon.add(rX, 0);
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