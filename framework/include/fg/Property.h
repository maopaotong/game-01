#pragma once
#include <string>
#include <vector>
#include <typeindex>
#include "fg/VarBag.h"
#include "fg/Global.h"

#include <vector>
#include <memory>
#include <type_traits>
#include <functional>
#include "Options.h"

namespace fog
{
    class Property
    {

    public:
        template <typename T>
        class Ref
        {
            std::string name;
            T *ptr_ = nullptr;

            Listener<Ref<T> &> *listener = nullptr;

            void doBind(T &ref)
            {
                doBind(std::addressof(ref));
            }
            void doBind(T *ptr)
            {
                ptr_ = ptr;
                if (listener)
                {
                    listener->onEvent(*this);
                }
            }

            void doSet(T value)
            {
                *ptr_ = value;
                if (listener)
                {
                    listener->onEvent(*this);
                }
            }

        public:
            Ref() {

            };

            Ref(T &&) = delete;
            Ref(T &ref)
            {
                ptr_ = std::addressof(ref);
            };
            Ref(const Ref &other) : ptr_(other.ptr_) {};
            Ref(Ref &&other) : ptr_(other.ptr_) {};

            std::string getName()
            {
                return name;
            }

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

            Ref &operator=(T &&value)
            {
                assertBind() = value;
                return *this;
            }

            Ref &operator=(const T value)
            {
                assertBind() = value;
                return *this;
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
            std::unordered_map<std::string, std::vector<std::function<void(Options::Option*)>>> createListener;

        public:
            template <typename T>
            Property::Ref<T> createProperty(std::string name, T defaultValue)
            {
                Property::Ref<T> ref = tryCreateProperty<T>(name, defaultValue);
                if (ref.empty())
                {
                    throw std::runtime_error("cannot create property ref with name:" + name);
                }
                return ref;
            }
            template <typename T>
            Property::Ref<T> tryCreateProperty(std::string name, T defaultValue)
            {
                Options::Option *opt = options.tryAdd<T>(name, defaultValue);
                return toPropertyRef<T>(opt);
            }

            template <typename T>
            Property::Ref<T> toPropertyRef(Options::Option *opt)
            {
                if (opt)
                {
                    T &ref = opt->getValueRef<T>();
                    return Property::Ref<T>(ref);
                }
                else
                {
                    
                    return Property::Ref<T>(); // empty ref.
                }
            }

            template <typename T>
            Property::Ref<T> getProperty(std::string name, bool requiredNow = true)
            {
                Options::Option *opt = options.getOption(name);
                if (!opt)
                {
                    if (requiredNow)
                    {
                        throw std::runtime_error("property ref not exists by name:" + name);
                    }


                }

                return toPropertyRef<T>(opt);
            }
            template <typename F>
            void forEach(F &&func)
            {
                options.forEach(std::forward<F>(func));
            }
        };
    };
}; //