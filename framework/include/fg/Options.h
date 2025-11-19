#pragma once
#include <string>
#include <vector>
#include <typeindex>
#include "fg/VarBag.h"
#include "fg/Global.h"
#include <memory>

namespace fog
{

    class Options
    {
    public:
        struct Option
        {
            std::string name;
            std::any valuePtr;
            std::function<void()> destructor;

            ~Option()
            {
                destructor();
            }
            template <typename T>
            Option(std::string name, T defaultV) : name(name)
            {
                T *valueP = new T(defaultV);
                this->valuePtr = std::make_any<T *>(valueP);
                this->destructor = [valueP]()
                {
                    delete valueP;
                };
            }

            Option(const Option &) = delete;            // copy
            Option(Option &&) = delete;                 // move
            Option &operator=(const Option &) = delete; // copy assign
            Option &operator=(Option &&) = delete;      // move assign

            template <typename T>
            bool isType() const
            {
                return valuePtr.type() == typeid(T *);
            }

            template <typename T>
            T &getValueRef() const
            {
                T *p = std::any_cast<T *>(valuePtr);
                return *p;
            }
        };

    protected:
        std::unordered_map<std::string, std::unique_ptr<Option>> options;

    public:
        ~Options()
        {
        }

        Option *getOption(std::string name)
        {
            auto &it = options.find(name);
            if(it==options.end()){
                return nullptr;
            } 
            return it->second.get();
        }

        template <typename T>
        Option *add(std::string name, T defaultValue)
        {
            Option *ret = tryAdd(name, defaultValue);

            if (ret)
            {
                return ret;
            }
            throw new std::runtime_error("cannot create options with name:" + name);
        }

        template <typename T>
        Option *tryAdd(std::string name, T defaultValue)
        {
            Option *opt = nullptr;
            auto it = options.find(name);
            if (it == options.end())
            {
                std::unique_ptr<Option> optionPtr = std::make_unique<Option>(name, defaultValue);
                //<std::unique_ptr<Option>>
                opt = optionPtr.get();
                options[name] = std::move(optionPtr);
            }

            return opt;
        }

        template <typename F>
        void forEach(F &&visit)
        {
            for (const auto &pair : options)
            {
                visit(pair.first, pair.second.get());
            }
        }
    };

};