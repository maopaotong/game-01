#pragma once
#include "imgui.h"
#include <string>
#include "fg/Core.h"
#include "fg/State.h"
#include "fg/Ground.h"
#include "fmt/format.h"
#include "fg/util/ImGuiUtil.h"

namespace fog
{

    class UIState
    {
    protected:
        UIState *pState;
        std::vector<UIState *> children;
        bool active = false;
        std::string name;

        void openChildren()
        {
            for (auto cState : children)
            {

                if (cState->active)
                {
                    cState->open();
                }
            }
        };

    public:
        UIState(UIState *pState, std::string name) : pState(pState), name(name)
        {
        }
        virtual ~UIState()
        {
        }
        UIState(UIState &&) = delete;
        UIState(UIState &) = delete;
        UIState &operator=(UIState &&) = delete;
        UIState &operator=(UIState &) = delete;

        virtual void init() {}
        std::string getName()
        {
            return name;
        }
        void changeActive()
        {
            this->setActive(!this->active);
        }
        void setActive(bool active)
        {
            this->active = active;
        }
        bool *activePtr()
        {
            return &this->active;
        }
        bool isActive()
        {
            return active;
        }
        void add(UIState *child)
        {

            children.push_back(child);
        }

        virtual bool open()
        {
            if (!this->active)
            {
                return false;
            }
            if (!ImGui::Begin(name.c_str(), &this->active))
            {
                return false;
            }
            doOpen();
            this->openChildren();
            ImGui::End();
            return true;
        };
        virtual void doOpen()
        {
        }
    };

}; // end of namespace