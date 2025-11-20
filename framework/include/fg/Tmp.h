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
        struct LaterBind
        {
            std::any ptr;

            template <typename T>
            T *getPtr()
            {
                if (!ptr.has_value())
                {
                    return nullptr;
                }
                return std::any_cast<T *>(ptr);
            }

            template <typename T>
            void setPtr(T *ptr_)
            {
                this->ptr = ptr_;
            }
        };

        template <typename T>
        class Ref
        {
            std::string name;
            T *ptr_ = nullptr;
            std::shared_ptr<LaterBind> later;

            void doBind(T &ref)
            {
                doBind(std::addressof(ref));
            }
            void doBind(T *ptr)
            {
                ptr_ = ptr;
            }

            void doSet(T value)
            {
                *ptr_ = value;
            }

        public:
            Ref(std::shared_ptr<LaterBind> later) : later(later) {

                                                    };

            Ref(T &&) = delete;
            Ref(T &ref)
            {
                ptr_ = std::addressof(ref);
            };
            Ref(const Ref &other) : ptr_(other.ptr_) {};
            Ref(Ref &&other) : ptr_(other.ptr_) {};
            Ref(T *ptr) : ptr_(ptr)
            {
            }
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
                this->ptr_ = other.ptr_;
                this->later = other.later;
                return *this;
            }

            Ref &operator=(Ref &&other)
            {
                this->ptr_ = other.ptr_;
                this->later = other.later;
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

            bool emptyNow() const { return ptr_ == nullptr; }

            bool emptyNorOrLater()
            {
                return ptr_ == nullptr && !later;
            }

            operator T &() &
            {
                return assertBind();
            }

            operator const T &() const &
            {
                return assertBind();
            }

            T &assertBind() const
            {
                if (ptr_)
                {
                    return *ptr_;
                }
                if (later && later->ptr)
                {

                    ptr_ = later->ptr;
                    later = nullptr;
                    return *ptr_;
                }

                throw std::runtime_error("property ref is empty or not available for now.");
            }
        };

        class Bag
        {
            Options options;
            std::unordered_map<std::string, std::vector<std::shared_ptr<LaterBind>>> laterBinds;

        public:
            template <typename T>
            Property::Ref<T> doCreateProperty(std::string name, Options::Option *opt)
            {
                // notify other later ref.
                auto it = laterBinds.find(name);
                if (it != laterBinds.end())
                {
                    for (auto laterI : laterBinds[name])
                    {
                        laterI->setPtr(opt);
                    }
                    laterBinds.erase(name);//release the later lister.
                }

                return Property::Ref<T>(opt);//no later for this ref.
            }

            template <typename T>
            Property::Ref<T> createProperty(std::string name, T defaultValue)
            {

                Options::Option *opt = options.tryAdd<T>(name, defaultValue);
                if (!opt)
                {
                    throw std::runtime_error("cannot create property ref with name:" + name);
                }
                return doCreateProperty(name, opt);
            }
            template <typename T>
            Property::Ref<T> tryCreateProperty(std::string name, T defaultValue)
            {

                Options::Option *opt = options.tryAdd<T>(name, defaultValue);
                if (!opt)
                {
                    return nullptr;//no later no now.
                }
                return doCreateProperty(name, opt);

            }

            template <typename T>
            Property::Ref<T> toPropertyRef(std::string name, Options::Option *opt)
            {
                if (opt)
                {
                    T &ref = opt->getValueRef<T>();
                    return Property::Ref<T>(ref);
                }
                else
                {
                    
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
                    std::shared_ptr<LaterBind> lb = std::make_shared<LaterBind>();
                    laterBinds[name].push_back(lb);
                    return Property::Ref<T>(lb); // empty ref.
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