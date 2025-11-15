#pragma once

template <typename...Args>
class Listener
{
public:
    virtual bool onEvent(Args...args) = 0;
};

template <typename...Args>
class FunctionListener : public Listener<Args...>
{
    bool (*func)(Args...);

public:
    virtual ~FunctionListener() {}
    FunctionListener(bool(func)(Args... args))
    {
        this->func = func;
    }
    bool onEvent(Args...args) override
    {
        return func(args...);
    }
};
