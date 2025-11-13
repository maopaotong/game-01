
#pragma once
#include "Actor.h"
#include <unordered_map>
#include <functional>
class Global
{

    template <typename T>
    struct VarPtr
    {
        T *ptr;
        T min;
        T max;
        VarPtr(VarPtr &vp)
        {
            VarPtr(vp.ptr, vp.min, vp.max);
        }
        VarPtr(T *ptr, T min, T max) : ptr(ptr), min(min), max(max) {}
    };

private:
    Actor *activeActor = nullptr;
    std::unordered_map<std::string, VarPtr<float>> floatVarMap;

public:
    void setActiveActor(Actor *actor)
    {
        this->activeActor = actor;
    }
    Actor *getActiveActor()
    {
        return this->activeActor;
    }

    void bindVar(const std::string name, float *ptr, float min, float max)
    {
        this->floatVarMap.erase(name);
        this->floatVarMap.try_emplace(name, ptr, min, max);
    }

    void forEachVar(std::function<void(const std::string, float *, float, float)> func)
    {

        for (std::pair<const std::string&, const VarPtr<float>&> pair : floatVarMap)
        {
            func(pair.first, pair.second.ptr, pair.second.min, pair.second.max);
        }
    }
};