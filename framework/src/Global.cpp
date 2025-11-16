#include "fg/Global.h"

namespace fog
{
//If the var here not initialised, app must provide the value.

template <>Terrains *Global::Context<Terrains *>::default_ = nullptr;
template <>Terrains *Global::Context<Terrains *>::ctxObj = nullptr;
//
template <>ActorPropEC *Global::Context<ActorPropEC *>::default_ = nullptr;
template <>ActorPropEC *Global::Context<ActorPropEC *>::ctxObj = new ActorPropEC();

};