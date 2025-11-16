#pragma once
#include "util/CellMark.h"

namespace fog
{
class Movable
{
public:
    virtual bool setTargetCell(CellKey &cKey2) = 0;
};
};