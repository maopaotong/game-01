#pragma once

template <typename T>
class PropertyEvent
{
    T *state;
    const std::string &name;
public:

    PropertyEvent(T *s, const std::string &name) : state(s), name(name)
    {
    }

    PropertyEvent(PropertyEvent &se) : name(se.name), state(se.state)
    {
    }

    T *getState()
    {
        return state;
    }
    const std::string &getName()
    {
        return name;
    }
};