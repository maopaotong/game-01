
#pragma once
#include "Actor.h"
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "VarBag.h"
#include <any>

class Global : public VarBag<float>, public VarBag<long>, public VarBag<std::any>
{
    typedef void (*FloatVarVistFunc)(const std::string name, float *vPtr, VarBag<float>::VarRange<float> *range);

};