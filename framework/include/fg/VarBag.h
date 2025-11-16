
#pragma once
#include "Actor.h"
#include <unordered_map>
#include <unordered_set>
#include <functional>
template <typename T>
class VarBag
{
public:
    template <typename T>
    struct VarRange
    {
        T min;
        T max;
        VarRange(T min, T max) : min(min), max(max) {}
    };

    template <typename T>
    class VarListener
    {
    public:
        virtual void onVarChanged(const std::string name, T &ref) = 0;
    };

private:
    template <typename T>
    struct VarBind
    {
        T value;
        VarRange<T> *scope = nullptr;
        std::unordered_set<VarListener<T> *> listeners;
        VarBind(VarBind &vp)
        {
            VarPtr(vp.ptr);
        }
        VarBind(T value) : value(value)
        {
        }
        ~VarBind()
        {
            if (this->scope)
            {
                delete this->scope;
            }
        }
        void addListener(VarListener<T> *l)
        {
            listeners.insert(l);
        }
        void removeListener(VarListener<T> *l)
        {
            listener.erase(l);
        }

        void notify(const std::string &name)
        {
            for (auto it : listeners)
            {
                it->onVarChanged(name, value);
            }
        }
    };

private:
    std::unordered_map<std::string, VarBind<T> *> varMap;

public:
    void setVar(const std::string name, T value)
    {

        VarBind<T> *vbp = this->getBind(name);

        if (vbp)
        {
            vbp->value = value;
        }
        else
        {
            void (*func)(VarBind<T> *) = [](VarBind<T> *) {};

            vbp = this->createBind(name, value, func);
        }
        vbp->notify(name);
    }

    T *createBindVptr(const std::string name, T value, T min, T max)
    {
        void (*func)(VarBind<T> *, T, T) = [](VarBind<T> *bind, T min, T max)
        { bind->scope = new VarRange(min, max); };

        return createBindVptr<T, T>(name, value, func, min, max);
    }

    template <typename... Args>
    T *createBindVptr(const std::string name, T defVal, void (*initFunc)(VarBind<T> *, Args...), Args... args)
    {
        VarBind<T> *vbp = this->createBind<Args...>(name, defVal, initFunc, args...);
        return &vbp->value;
    }

    template <typename... Args>
    VarBind<T> *createBind(const std::string name, T defVal, void (*initFunc)(VarBind<T> *, Args...), Args... args)
    {
        VarBind<T> *vbp = this->getBind(name);
        if (vbp)
        {
            return vbp;
        }
        vbp = new VarBind<T>(defVal);
        this->varMap.emplace(name, vbp); //
        initFunc(vbp, args...);
        return vbp;
    }
    VarBind<T> *getBind(const std::string name)
    {
        std::unordered_map<std::string, VarBind<T> *>::iterator it = this->varMap.find(name);
        if (it == this->varMap.end())
        {
            return nullptr;
        }
        return it->second;
    }

    T *getVarPtr(std::string name)
    {
        VarBind<T> *vb = this->getBind(name);
        if (vb)
        {
            return &vb->value;
        }
        return nullptr;
    }

    bool getVarVal(std::string name, T &ret)
    {
        T *ptr = this->getVarPtr(name);
        if (ptr)
        {
            ret = *ptr;
            return true;
        }
        return false;
    }

    template <typename T2>
    T2 getVarVal(std::string name, T2 (*cvtFunc)(T from), T2 def)
    {
        T *ptr = this->getVarPtr(name);
        if (ptr)
        {
            return def;
        }
        return cvtFunc(*ptr);
    }

    T getVarVal(std::string name, T def)
    {
        T *ptr = this->getVarPtr(name);
        if (ptr)
        {
            return *ptr;
        }
        return def;
    }

    template <typename... Args>
    void forEachVarPtr(void (*func)(const std::string, T *, VarRange<T> *scope, Args... args), Args... args)
    {
        for (auto pair : varMap)
        {
            VarBind<T> *bind = pair.second;
            func(pair.first, &bind->value, bind->scope, args...);
        }
    }

    
    template <typename... Args>
    void forEachVarPtr(void (*func)(const std::string, T *, Args... args), Args... args)
    {
        for (auto pair : varMap)
        {
            VarBind<T> *bind = pair.second;
            func(pair.first, &bind->value, args...);
        }
    }
};