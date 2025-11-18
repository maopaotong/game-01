#pragma once
#include <string>
#include <vector>
#include <typeindex>
#include "fg/VarBag.h"
#include "fg/Global.h"

#include <vector>
#include <memory>
#include <type_traits>
namespace fog
{
    class Property
    {
    protected:
        struct BaseWatcher
        {
            virtual ~BaseWatcher() = default;
            virtual void checkAndEmit() = 0;
        };
        template <typename T, typename U>
        class Watcher : public BaseWatcher
        {
            T *obj_;
            U T::*member_;
            U lastValue_;
            bool hasLast_ = false;

        public:
            Watcher(T *obj, U T::*mem)
                : obj_(obj), member_(mem) {}

            void checkAndEmit() override
            {
                if (!obj_)
                    return; // 防空指针

                const U &currentValue = obj_->*member_;

                if (!hasLast_ || currentValue != lastValue_)
                {
                    lastValue_ = currentValue;
                    hasLast_ = true;
                    EventBus::emit(currentValue); // emit(U)
                }
            }
        };

    public:
        class Monitor
        {

            std::vector<std::unique_ptr<BaseWatcher>> watchers_;

        public:
            template <typename T, typename U>
            void add(T *obj, U T::*member)
            {
                watchers_.emplace_back(std::make_unique<Watcher<T, U>>(obj, member));
            }

            void refresh()
            {
                for (auto &w : watchers_)
                {
                    w->checkAndEmit();
                }
            }

        private:
        };
    };

};