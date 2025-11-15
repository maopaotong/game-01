#pragma once

template <typename T>
class Listener
{
public:
    virtual bool onEvent(T &evt) = 0;
};

template <typename T>
class FunctionListener : public Listener<T>
{
    std::function<bool(T)> func;

public:
    virtual ~FunctionListener() {}
    FunctionListener(std::function<bool(T &)> func)
    {
        this->func = func;
    }
    bool onEvent(T &evt) override
    {
        return func(evt);
    }
};
