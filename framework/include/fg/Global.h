
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

    template<typename T>
    class VarListener
    {
    public:
        virtual void onVarChanged(const std::string name, T& ref) = 0;
    };

private:
    template <typename T>
    struct VarBind
    {
        T value;
        VarScope<T> *scope = nullptr;
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

        void notify(const std::string& name)
        {
            for (auto it : listeners)
            {
                it->onVarChanged(name, value);
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
        vb->notify(name);
    }
    void setVar(const std::string name, float value)
    {

        VarBind<float> *vb = this->getOrCreate(name, value);
        vb->value = value;
        vb->notify(name);
    }

    VarBind<float> *getOrCreate(const std::string name, float value)
    {
        std::unordered_map<std::string, VarBind<float> *>::iterator it = this->floatVarMap.find(name);
        VarBind<float> *vp;
        if (it == this->floatVarMap.end())
        {
            vp = new VarBind<float>(value);
            this->floatVarMap.emplace(name, vp); //
        }
        else
        {
            vp = it->second;
        }

        return vp;
    }

    float *getVarPtr(std::string name)
    {
        return &this->getOrCreate(name, 0.0f)->value;
    }
    float &getVarRef(std::string name)
    {
        return *this->getVarPtr(name);
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