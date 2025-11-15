#pragma once
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <any>
#include <functional>
#include <typeindex>

#include "fg/Listener.h"

template <typename T>
class EventCenter
{
    std::vector<Listener<T> *> listeners;

public:
    void addListener(Listener<T> *listener)
    {
        listeners.push_back(listener);
    }

    void emit(T &evt)
    {
        for (auto listener : listeners)
        {
            listener->onEvent(evt);
        }
    }

    template <typename T>
    void addListener(bool (*func)(T &))
    {
        addListener(new FunctionListener<T&>(func));
    }
};