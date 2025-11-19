#pragma once
#include <string>
#include <vector>
#include <typeindex>
#include "fg/VarBag.h"
#include "fg/Global.h"

#include <vector>
#include <memory>
#include <type_traits>
#include "Options.h"

namespace fog
{
    class Property
    {

    public:
        template <typename T>
        class Ref
        {
            T *ptr_ = nullptr;

            void doSet(T value)
            {
                *ptr_ = value;
            }

            void doBind(T &ref)
            {
                doBind(std::addressof(ref));
            }
            void doBind(T *ptr)
            {
                ptr_ = ptr;
            }

        public:
            Ref() {

            };

            Ref(T &&) = delete;
            Ref(T &ref)
            {
                ptr_ = std::addressof(ref);
            };
            Ref(const Ref &other)
            {
                ptr_ = other->ptr_;
            };
            Ref(Ref &&other)
            {
                ptr_ = other->ptr_;
            };

            void set(T value)
            {
                assertBind() = value;
            }

            void set(T &&value)
            {
                assertBind() = value;
            }

            Ref &operator=(const Ref &other)
            {
                return bindOrSet();
            }

            Ref &operator=(Ref &&other)
            {
                if (this->empty())
                {
                    doBind(other.ptr_);
                }
                else
                {
                    if (!other.empty())
                    {
                        this->doSet(*other.ptr_);
                    }
                    else
                    {
                        // do nothing.
                    }
                }
                return *this;
            }

            Ref &operator=(T &ref)
            {
                return bindOrSet(ref);
            }

            Ref &bindOrSet(T &ref)
            {

                if (empty())
                {
                    doBind(ref);
                }
                else
                {
                    doSet(ref);
                }
                return *this;
            }

            Ref &operator=(T &&value)
            {
                assertBind() = value;
                return *this;
            }

            void bind(T &ref)
            {
                assertEmpty();
                doBind(ref);
            }

            bool empty() const { return ptr_ == nullptr; }

            operator T &() &
            {
                return assertBind();
            }

            operator const T &() const &
            {
                return assertBind();
            }

            void assertEmpty()
            {
                if (ptr_)
                {
                    throw std::runtime_error("property ref not empty.");
                }
            }

            T &assertBind() const
            {
                if (!ptr_)
                {
                    throw std::runtime_error("property ref is empty.");
                }
                return *ptr_;
            }
        };

        class Bag
        {
            Options options;
            template <typename T>
            std::reference_wrapper<T> createProperty(std::string name, T defaultValue)
            {
                Option *opt = options.add<T>(name, defaultValue);
            }
        };

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