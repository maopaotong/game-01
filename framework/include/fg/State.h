#pragma once

#include <Ogre.h>
#include <OgreNode.h>
#include <type_traits>
#include <functional>
#include <iostream>
#include "PathFollow2.h"
#include "Pickable.h"
#include "OgreFrameListener.h"
#include "Movable.h"
#include "fg/Event.h"
#include "fg/Context.h"
#include "fg/Global.h"
#include "fg/MeshBuild.h"
#include "fg/Property.h"
#include "fg/Options.h"

#define WATCH_PROPERTY(monitor, obj_ptr, member_name) \
    (monitor).add((obj_ptr), &std::remove_pointer_t<decltype(obj_ptr)>::member_name, #member_name)

namespace fog
{
    using namespace Ogre;

    class State
    {
    public:
        static State *get(Node *node)
        {
            const Any &any = node->getUserAny();
            if (any.isEmpty())
            {
                return nullptr;
            }

            State *state = Ogre::any_cast<State *>(any);
            return state;
        }
        static void set(SceneNode *node, State *state)
        {
            node->setUserAny(state);
        }

        static State *getState(MovableObject *mo)
        {
            const Any &any = mo->getUserAny();
            if (any.isEmpty())
            {
                return nullptr;
            }

            State *state = Ogre::any_cast<State *>(any);
            return state;
        }

    protected:
        State *parent = nullptr;
        FrameListener *frameListener = nullptr;
        Pickable *pickable = nullptr;
        SceneNode *sceNode = nullptr;
        std::vector<State *> *children = nullptr;
        bool active = false;
        Options options;

        template <typename T>
        Property::Ref<T> createProperty(std::string name, T defaultValue)
        {
            return Context<Property::Bag *>::get()->createProperty<T>(name, defaultValue);
        }

        template <typename T>
        Property::Ref<T> getProperty(std::string name)
        {
            return Context<Property::Bag *>::get()->getProperty<T>(name);
        }

    public:
        State()
        {
            this->children = new std::vector<State *>();
            std::cout << "new State()" << this << "" << std::endl;
        }
        virtual ~State()
        {
            std::cout << "~State()" << this << "" << std::endl;
        }
        virtual Task::Owner * createTaskOwner() {
            return nullptr;
        }
        virtual std::type_index getTaskOwnerType()
        {
            return std::type_index(typeid(nullptr));
        }

        virtual void init() {

        };

        virtual void collectProperties()
        {
        }

        Vector3 to3D(Vector2 point)
        {
            return Context<Node2D *>::get()->to3D(point);
        }

        bool findCell(Vector3 aPos3, Cell::Instance &cell)
        {
            return Context<Cell::Center *>::get()->findCellByWorldPosiion(aPos3, cell);
        }

        SceneNode *getSceneNode()
        {
            return this->sceNode;
        }

        SceneNode *findSceneNode()
        {
            State *s = this;
            while (s)
            {
                if (s->sceNode)
                {
                    return s->sceNode;
                }
                s = s->parent;
            }
            return nullptr;
        }
        void setSceneNode(SceneNode *sNode)
        {
            State::set(sNode, this);
            this->sceNode = sNode;
        }

        void addChild(State *s)
        {
            if (s->parent)
            {
                throw std::runtime_error("Already has a parent state.");
            }
            this->children->push_back(s);
            s->parent = this;
        }

        void removeChild(State *cs)
        {
            this->children->erase(std::remove(this->children->begin(), this->children->end(), cs), this->children->end());
        }

        Pickable *getPickable()
        {
            return this->pickable;
        }

        void setPickable(Pickable *pick)
        {
            this->pickable = pick;
        }

        FrameListener *getFrameListener()
        {
            return this->frameListener;
        }

        void setFrameListener(FrameListener *listener)
        {
            this->frameListener = listener;
        }

        void setActive(bool active)
        {
            bool changed = (this->active != active);
            this->active = active;
            if (changed)
            {

                // Context<ActorPropEC *>::get()->emit(this, std::string("active"));
                Context<Event::Bus *>::get()->emit<State *, std::string &>(this, std::string("active"));
            }
        }

        bool isActive()
        {
            return this->active;
        }

        template <typename... Args>
        void forEachChild(void (*func)(State *, Args...), Args... args)
        {
            forEachChild<Args...>(true, func, args...);
        }

        template <typename F>
        void forEachChild(F &&func, bool recursive = true)
        {
            std::vector<State *> *tmp = this->children;
            for (auto it = tmp->begin(); it != tmp->end(); ++it)
            {
                State *s = *it;
                bool brk = func(s);
                if (brk)
                {
                    break;
                }
                if (recursive)
                {
                    s->forEachChild(func);
                }
            }
        }

        template <typename... Args>
        void forEachChild(bool recursive, void (*func)(State *, Args...), Args... args)
        {
            std::vector<State *> *tmp = this->children;
            for (auto it = tmp->begin(); it != tmp->end(); ++it)
            {
                State *s = *it;
                func(s, args...);
                if (recursive)
                {
                    s->forEachChild<Args...>(true, func, args...);
                }
            }
        }

    public:
    };
};