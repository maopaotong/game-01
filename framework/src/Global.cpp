#include "fg/Global.h"

template <>Terrains *Global::Context<Terrains *>::default_ = nullptr;
template <>Terrains *Global::Context<Terrains *>::ctxObj = nullptr;
//
template <>ECPEActor *Global::Context<ECPEActor *>::default_ = nullptr;
template <>ECPEActor *Global::Context<ECPEActor *>::ctxObj = new ECPEActor();