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
            std::type_index type;

            Option(std::string name, std::any pValue, std::type_index type) : name(name), valuePtr(pValue), type(type)
            {
            }
            template <typename T>
            bool isType()
            {
                return type == typeid(T);
            }

            template <typename T>
            T *getValuePtr()
            {
                return std::any_cast<T *>(valuePtr);
            }
        };

    protected:
        std::unordered_map<std::string, std::unique_ptr<Option>> options;
    public:
        ~Options(){

        }        

        Option *getOption(std::string name)
        {
            return options[name].get();
        }

        template <typename T>
        void add(std::string name, T defaultValue)
        {
            T *pValue = new T();
            *pValue = defaultValue;
            Option *option = new Option(name, std::make_any<T *>(pValue), typeid(T *));
            options[name] = std::make_unique<Option>(option);
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