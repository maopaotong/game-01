#include "fg/Global.h"

namespace fog
{
    // If the var here not initialised, app must provide the value.

    template <>
    Terrains *Global::Context<Terrains *>::default_ = nullptr;
    template <>
    Terrains *Global::Context<Terrains *>::ctxObj = nullptr;
    //
    template <>
    ActorPropEC *Global::Context<ActorPropEC *>::default_ = nullptr;
    template <>
    ActorPropEC *Global::Context<ActorPropEC *>::ctxObj = new ActorPropEC();
    //
    CostMap *costMap = new CostMap(30, 25);
    Node2D *node2D = new Node2D();
    //
    template <>
    Cell::Center *Global::Context<Cell::Center *>::default_ = nullptr;
    template <>
    Cell::Center *Global::Context<Cell::Center *>::ctxObj = new Cell::Center(node2D, costMap);

};