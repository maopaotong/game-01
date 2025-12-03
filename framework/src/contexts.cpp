#include "fg/Context.h"
#include "fg/Property.h"
#include "fg/CoreMod.h"
#include "fg/CellInstanceManager.h"
#include "fg/MovingStateManager.h"
#include "fg/MovableStateManager.h"
#include "fg/PathingStateManager.h"
#include "fg/demo/InputStateController.h"
#include "fg/BuildingStateManager.h"
#include "fg/InventoryStateManager.h"
#include "fg/CameraState.h"
namespace fog
{
    // If the var here not initialised, app must provide the value.
    //

    template <>
    Var<float>::Bag *Context<Var<float>::Bag>::ctxObj = new Var<float>::Bag();

    template <>
    Var<Vector3>::Bag *Context<Var<Vector3>::Bag>::ctxObj = new Var<Vector3>::Bag();

    template <>
    Property::Bag *Context<Property::Bag>::ctxObj = new Property::Bag();

    template <>
    Terrains *Context<Terrains>::ctxObj = nullptr;

    template <>
    Event::Bus *Context<Event::Bus>::ctxObj = new Event::Bus();

    template <>
    Node2D *Context<Node2D>::ctxObj = nullptr;

    template <>
    Cell::Center *Context<Cell::Center>::ctxObj = nullptr;

    template <>
    CoreMod *Context<CoreMod>::ctxObj = nullptr;

    template <>
    CostMap *Context<CostMap>::ctxObj = nullptr;
    //
    template <>
    State *Context<State>::ctxObj = nullptr;

    template <>
    Plane *Context<Plane>::ctxObj = nullptr;

    template <>
    MovingStateManager *Context<MovingStateManager>::ctxObj = new MovingStateManager();

    template <>
    CellInstanceManager *Context<CellInstanceManager>::ctxObj = new CellInstanceManager();

    template <>
    MovableStateManager *Context<MovableStateManager>::ctxObj = new MovableStateManager();

    template <>
    PathingStateManager *Context<PathingStateManager>::ctxObj = new PathingStateManager();

    template <>
    InputStateController *Context<InputStateController>::ctxObj = new InputStateController();

    template <>
    BuildingStateManager *Context<BuildingStateManager>::ctxObj = new BuildingStateManager();

    template <>
    InventoryStateManager *Context<InventoryStateManager>::ctxObj = new InventoryStateManager();
    
    template <>
    CameraState *Context<CameraState>::ctxObj = new CameraState();
    
};