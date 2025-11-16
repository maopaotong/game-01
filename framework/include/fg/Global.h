
#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "VarBag.h"
#include "fg/Terrains.h"
#include "fg/Actor.h"
#include "fg/EventCenter.h"
#include "fg/PropertyEvent.h"
#include <string>

template <typename T>
class Context
{
private:
public:
    static T default_;
    static T ctxObj;

    static T get()
    {
        return ctxObj;
    }
    static void setDefault(T obj)
    {
        default_ = obj;
    }

    static void set(T obj)
    {
        ctxObj = obj;
    }

    static T unset()
    {
        T previousCtxObj = ctxObj;
        ctxObj = default_;
        return previousCtxObj;
    }

    template <typename R, typename... Args>
    static R callWithContext(T ctxObj, R (*func)(), Args... args)
    {
        R ret;
        runWithContext<T, Args...>(ctxObj, [](R (*func)(), &ret, Args... args)
                                   { ret = func(args...); }, func, ret, args...);
        return ret;
    }

    template <typename... Args>
    static void runWithContext(T ctxObj, void (*func)(), Args... args)
    {
        T previousCtxObj = Context<T>::get();
        Context<T>::set(ctxObj);
        func(args...);
        Context<T>::set(previousCtxObj);
    }
};

typedef EventCenter<Actor*, std::string &> ECActorProperty;

class Global : public VarBag<float>, public VarBag<long>, public VarBag<Vector3>, public VarBag<std::any>, //
               public Context<Terrains *>,                                                                 //
               public Context<ECActorProperty *>                                                                 //
{
    typedef void (*FloatVarVistFunc)(const std::string name, float *vPtr, VarBag<float>::VarRange<float> *range);

public:
    static float getTerrainHeightAtPositionWithOffset(float x, float y, float offset = 0.0f)
    {
        return Context<Terrains *>::get()->getHeightAtPosition(x, y) + offset;
    }
};