#pragma once
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <any>
#include <functional>
#include <typeindex>

#include <vector>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <memory>

#include "fg/Listener.h"

namespace fog
{

    class Event
    {
    public:
        class Mask
        {
        };

        template <typename... Args>
        class Center
        {
            std::vector<Listener<Args...> *> listeners;

        public:
            void addListener(Listener<Args...> *listener)
            {
                listeners.push_back(listener);
            }

            void emit(Args... evt)
            {
                for (auto listener : listeners)
                {
                    listener->onEvent(evt...);
                }
            }
        };

        class Bus
        {
        public:
            // template <typename... Args>
            // void subscribe(std::function<void(Args...)> callback)
            // {
            //     getCallbacks<Args...>().push_back(std::move(callback));
            // }

            template <typename... Args, typename F>
            void subscribe(F &&func)
            {
                //
                getCallbacks<Args...>().emplace_back(std::forward<F>(func));
            }

            // template <typename... Args>
            // void subscribe(Listener<Args...> *callback)
            // {
            //     std::function<void(Args...)> func = [callback](Args... args)
            //     {
            //         callback->onEvent(args...);
            //     };
            //     subscribe<Args...>(func);
            // }

            template <typename... Args>
            void emit(Args... args)
            {
                for (auto &cb : getCallbacks<Args...>())
                {
                    cb(args...);
                }
            }

        private:
            // 类型擦除：用基类指针统一管理不同事件类型的回调列表
            struct CallbackBase
            {
                virtual ~CallbackBase() = default;
            };

            template <typename... Args>
            struct CallbackList : CallbackBase
            {
                std::vector<std::function<void(Args...)>> callbacks;
            };

            std::unordered_map<std::type_index, std::unique_ptr<CallbackBase>> registry;

            template <typename... Args>
            std::vector<std::function<void(Args...)>> &getCallbacks()
            {
                std::type_index ti = std::type_index(typeid(std::tuple<Args...>));
                auto it = registry.find(ti);
                if (it == registry.end())
                {
                    auto list = std::make_unique<CallbackList<Args...>>();
                    auto &ref = list->callbacks;
                    registry[ti] = std::move(list);
                    return ref;
                }
                return static_cast<CallbackList<Args...> *>(it->second.get())->callbacks;
            }
        };
    };
};