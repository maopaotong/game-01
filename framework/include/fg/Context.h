
#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "VarBag.h"
#include "fg/Terrains.h"
#include "fg/Actor.h"
#include "fg/Event.h"
#include <string>
namespace fog
{
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

        static T exchange(T obj)
        {
            return std::exchange<T>(ctxObj, obj);
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
        template <typename... Args, typename F>
        static void runWithContext(T ctxObj, F &&func, Args... args)
        {
            T previousCtxObj = exchange(ctxObj);
            func(args...);
            set(previousCtxObj);
        }
    };
};