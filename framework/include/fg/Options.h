#pragma once
#include <string>
#include <vector>
#include <typeindex>
#include "fg/VarBag.h"
#include "fg/Global.h"
namespace fog
{
    class Options
    {
    public:
        struct Option
        {
            std::string name;
            std::any pValue;
            std::type_index type;

            Option(std::string name, std::any pValue, std::type_index type) : name(name), pValue(pValue), type(type)
            {
            }
            template <typename T>
            bool isType()
            {
                return type == typeid(T);
            }

            template <typename T>
            T* getValuePtr()
            {
                return std::any_cast<T*>(pValue);
            }
        };

        std::unordered_map<std::string, Option *> options;

        Option * getOption(std::string name){
            return options[name];
        }

        template <typename T>
        void add(std::string name, T defaultValue)
        {
            T *pValue = new T();
            *pValue = defaultValue;
            Option *option = new Option(name, std::make_any<T*>(pValue), typeid(T *));
            options[name] = option;
        }

        template <typename F>
        void forEach(F &&visit)
        {
            for (auto pair : options)
            {
                visit(pair.first, pair.second);
            }
        }
    };

};