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
#include "fg/Global.h"

namespace fog
{
    using namespace Ogre;

    class State : public Actor
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
        Movable *movable = nullptr;
        SceneNode *sceNode = nullptr;
        std::vector<State *> *children = nullptr;
        bool active = false;

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

        virtual CellKey getDestinationCell() override
        {
            return CellKey(0, 0);
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

        Movable *getMovable()
        {
            return this->movable;
        }

        void setMovable(Movable *mvb)
        {
            this->movable = mvb;
        }

        void setActive(bool active)
        {
            bool changed = (this->active != active);
            this->active = active;
            if (changed)
            {
                Global::Context<ActorPropEC *>::get()->emit(this, std::string("active"));
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