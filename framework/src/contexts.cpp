#include "fg/Context.h"
#include "fg/Property.h"
#include "fg/CoreMod.h"
#include "fg/CellInstanceManager.h"
#include "fg/MovingStateManager.h"
#include "fg/MovableStateManager.h"
#include "fg/PathingStateManager.h"
namespace fog
{
    // If the var here not initialised, app must provide the value.
    //

    template <>
    Var<float>::Bag *Context<Var<float>::Bag *>::default_ = nullptr;

    template <>
    Var<float>::Bag *Context<Var<float>::Bag *>::ctxObj = new Var<float>::Bag();

    template <>
    Var<Vector3>::Bag *Context<Var<Vector3>::Bag *>::default_ = nullptr;

    template <>
    Var<Vector3>::Bag *Context<Var<Vector3>::Bag *>::ctxObj = new Var<Vector3>::Bag();

    template <>
    Property::Bag *Context<Property::Bag *>::default_ = nullptr;

    template <>
    Property::Bag *Context<Property::Bag *>::ctxObj = new Property::Bag();

    template <>
    Terrains *Context<Terrains *>::default_ = nullptr;
    template <>
    Terrains *Context<Terrains *>::ctxObj = nullptr;

    template <>
    Event::Bus *Context<Event::Bus *>::default_ = nullptr;
    template <>
    Event::Bus *Context<Event::Bus *>::ctxObj = new Event::Bus();

    //
    template <>
    Node2D *Context<Node2D *>::default_ = nullptr;
    template <>
    Node2D *Context<Node2D *>::ctxObj = nullptr;

    //
    template <>
    Cell::Center *Context<Cell::Center *>::default_ = nullptr;
    template <>
    Cell::Center *Context<Cell::Center *>::ctxObj = nullptr;
    

    //Core context used during the setup stage,no need to provide the core for event handling.
    template <>
    CoreMod *Context<CoreMod *>::default_ = nullptr;
    template <>
    CoreMod *Context<CoreMod *>::ctxObj = nullptr;

    // TODO remove this app level contex.
    template <>
    CostMap *Context<CostMap *>::default_ = nullptr;
    template <>
    CostMap *Context<CostMap *>::ctxObj = nullptr;
    //
    template <>
    State *Context<State *>::default_ = nullptr;
    template <>
    State *Context<State *>::ctxObj = nullptr;

    template <>
    Plane *Context<Plane *>::default_ = nullptr;
    template <>
    Plane *Context<Plane *>::ctxObj = nullptr;

    template <>
    MovingStateManager *Context<MovingStateManager *>::default_ = nullptr;
    template <>
    MovingStateManager *Context<MovingStateManager *>::ctxObj = new MovingStateManager();

    //
        template <>
    CellInstanceManager *Context<CellInstanceManager *>::default_ = nullptr;
    template <>
    CellInstanceManager *Context<CellInstanceManager *>::ctxObj = new CellInstanceManager();

    
        template <>
    MovableStateManager *Context<MovableStateManager *>::default_ = nullptr;
    template <>
    MovableStateManager *Context<MovableStateManager *>::ctxObj = new MovableStateManager();

        template <>
    PathingStateManager *Context<PathingStateManager *>::default_ = nullptr;
    template <>
    PathingStateManager *Context<PathingStateManager *>::ctxObj = new PathingStateManager();

    
    
};