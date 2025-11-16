#include "fg/Global.h"

//If the var here not initialised, app must provide the value.

template <>Terrains *Global::Context<Terrains *>::default_ = nullptr;
template <>Terrains *Global::Context<Terrains *>::ctxObj = nullptr;
//
template <>ECActorProperty *Global::Context<ECActorProperty *>::default_ = nullptr;
template <>ECActorProperty *Global::Context<ECActorProperty *>::ctxObj = new ECActorProperty();