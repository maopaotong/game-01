#pragma once
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <any>
#include <functional>
#include <typeindex>

#include "fg/Listener.h"

class EventCenter
{
    std::unordered_map<std::type_index, std::any> eventsMap;

public:
    template <typename T>
    static void emit(EventCenter *ec, T &evt)
    {
        if (ec)
        {
            ec->emit(evt);
        }
    }

    template <typename T>
    class SubCenter
    {
        std::vector<Listener<T> *> listeners;

    public:
        void addListener(Listener<T> *listener)
        {
            listeners.push_back(listener);
        }

        void notify(T &evt)
        {
            for (auto listener : listeners)
            {
                listener->onEvent(evt);
            }
        }
    };

    template <typename T>
    SubCenter<T> *getSubCenter()
    {
        std::type_index typeId = typeid(T);
        auto it = eventsMap.find(typeId);
        if (it != eventsMap.end())
        {
            return std::any_cast<SubCenter<T> *>(it->second);
        }
        return nullptr;
    }

    template <typename T>
    void addListener(std::function<bool(T &)> func)
    {
        addListener(new FunctionListener<T>(func));
    }

    template <typename T>
    void addListener(Listener<T> *listener)
    {
        std::type_index typeId = typeid(T);
        SubCenter<T> *events = getSubCenter<T>();
        auto it = eventsMap.find(typeId);
        if (it == eventsMap.end())
        {
            events = new SubCenter<T>();
            eventsMap[typeId] = events;
        }
        else
        {
            events = std::any_cast<SubCenter<T> *>(it->second);
        }
        events->addListener(listener);
    }

    template <typename T>
    void emit(T &evt)
    {
        SubCenter<T> *events = getSubCenter<T>();
        if (events)
        {
            events->notify(evt);
        }
    }
};
