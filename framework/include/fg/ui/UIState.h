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

    protected:
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
        UIState(UIState *pState) : pState(pState)
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
            return true;
        };
    };

}; // end of namespace