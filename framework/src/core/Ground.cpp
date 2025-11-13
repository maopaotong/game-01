#pragma once
#include "fg/Ground.h"
#include "fg/util/Polygon2.h"

class GroundImpl :public Ground
{
protected:
    Polygon2 polygon;

public:
    bool isPointInside(Vector2 &p)
    {
        return isPointInside(p.x, p.y);
    }
    bool isPointInside(float x, float y)
    {
        return polygon.isPointInPolygon(x, y);
    }
};
Ground::Ground(){

}