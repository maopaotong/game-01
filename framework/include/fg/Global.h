
#pragma once
#include "Actor.h"
#include <unordered_map>
#include <unordered_set>
#include <functional>
class Global
{
public:
    template <typename T>
    struct VarScope
    {
        T min;
        T max;
    };

private:
    template <typename T>
    struct VarBind
    {
        T value;
        VarScope<T> *scope = nullptr;
        std::unordered_set<T *> binds;
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
        void bind(T *ptr)
        {
            binds.insert(ptr);
            sync(ptr);
        }
        void unbind(T *ptr)
        {
            binds.erase(ptr);
        }
        void sync(T *ptr)
        {
            *ptr = this->value;
        }
        void sync()
        {
            for (auto it : binds)
            {
                sync(it);
            }
        }
    };

private:
    Actor *activeActor = nullptr;
    std::unordered_map<std::string, VarBind<float> *> floatVarMap;

public:
    void setActiveActor(Actor *actor)
    {
        this->activeActor = actor;
    }
    Actor *getActiveActor()
    {
        return this->activeActor;
    }

    void setVarAndScope(const std::string name, float value, float min, float max)
    {

        VarBind<float> *vb = this->getOrCreate(name, value);
        VarScope<float> *scope = vb->scope;
        if (!scope)
        {
            scope = new VarScope<float>();
            vb->scope = scope;
        }
        scope->min = min;
        scope->max = max;
        vb->value = value;
        vb->sync();
    }
    void setVar(const std::string name, float value)
    {

        VarBind<float> *vb = this->getOrCreate(name, value);
        vb->value = value;
        vb->sync();
    }

    VarBind<float> *getOrCreate(const std::string name, float value)
    {
        std::unordered_map<std::string, VarBind<float> *>::iterator it = this->floatVarMap.find(name);
        VarBind<float> *vp;
        if (it == this->floatVarMap.end())
        {
            vp = new VarBind<float>(value);
            this->floatVarMap.try_emplace(name, vp); //
        }
        else
        {
            vp = it->second;
        }

        return vp;
    }

    void bindVar(const std::string name, float *ptr)
    {
        VarBind<float> *vb = this->getOrCreate(name, *ptr);
        vb->bind(ptr);
    }

    void forEachVarPtr(std::function<void(const std::string, float *, VarScope<float> *scope)> func)
    {
        for (auto pair : floatVarMap)
        {
            VarBind<float> *bind = pair.second;
            func(pair.first, &bind->value, bind->scope);
        }
    }
};