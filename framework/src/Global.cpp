#include "fg/Global.h"

template <>Terrains *Global::Context<Terrains *>::default_ = nullptr;
template <>Terrains *Global::Context<Terrains *>::ctxObj = nullptr;