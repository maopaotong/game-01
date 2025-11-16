#pragma once
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <any>
#include <functional>
#include <typeindex>

#include "fg/Listener.h"

template <typename... Args>
class EventCenter
{
    std::vector<Listener<Args...> *> listeners;

public:
    void addListener(Listener<Args...> *listener)
    {
        listeners.push_back(listener);
    }

    void emit(Args...evt)
    {
        for (auto listener : listeners)
        {
            listener->onEvent(evt...);
        }
    }

    void addListener(bool (*func)(Args... args))
    {
        addListener(new FunctionListener<Args...>(func));
    }
};