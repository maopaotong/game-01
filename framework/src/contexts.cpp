#include "fg/Context.h"
#include "fg/Property.h"

namespace fog
{
    // If the var here not initialised, app must provide the value.
    //

    template <>
    Property::Bag *Context<Property::Bag *>::default_ = nullptr;

    template <>
    Property::Bag *Context<Property::Bag *>::ctxObj = new Property::Bag();

};