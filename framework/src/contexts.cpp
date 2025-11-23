#include "fg/Context.h"
#include "fg/Property.h"
#include "fg/Core.h"
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
    Var<Actor *>::Bag *Context<Var<Actor *>::Bag *>::default_ = nullptr;

    template <>
    Var<Actor *>::Bag *Context<Var<Actor *>::Bag *>::ctxObj = new Var<Actor *>::Bag();

    template <>
    Property::Bag *Context<Property::Bag *>::default_ = nullptr;

    template <>
    Property::Bag *Context<Property::Bag *>::ctxObj = new Property::Bag();

    template <>
    Terrains *Context<Terrains *>::default_ = nullptr;
    template <>
    Terrains *Context<Terrains *>::ctxObj = nullptr;

    //
    template <>
    ActorPropEC *Context<ActorPropEC *>::default_ = nullptr;
    template <>
    ActorPropEC *Context<ActorPropEC *>::ctxObj = new ActorPropEC();
    //
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
    Core *Context<Core *>::default_ = nullptr;
    template <>
    Core *Context<Core *>::ctxObj = nullptr;

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

    
    
};