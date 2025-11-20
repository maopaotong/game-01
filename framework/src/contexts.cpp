#include "fg/Context.h"
#include "fg/Property.h"
#include "fg/Master.h"
#include "fg/demo/DefaultMaster.h"

namespace fog
{
    // If the var here not initialised, app must provide the value.
    //

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
    //
    template <>
    Master *Context<Master *>::default_ = nullptr;

    template <>
    Master *Context<Master *>::ctxObj = nullptr;
    
    //
    
};