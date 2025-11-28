#pragma once
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <string>
#include <tuple>
#include <memory>
#include <optional>

#include <Ogre.h>
#include <OgreColourValue.h>

#define FOG_CAM_SPEED 750

namespace fog
{
    constexpr bool DEBUG_COUT = false;
    constexpr float TEXTURE_COORD_SCALE = 30.0f;

    using GOON = bool;

    using CONSUMED = bool;

    using Vector2 = Ogre::Vector2;

    using Vector3 = Ogre::Vector3;

    using ColourValue = Ogre::ColourValue;

    using String = std::string;

    template <typename T>
    using List = std::vector<T>;

    template <typename T>
    using UniquePtr = std::unique_ptr<T>;

    template <typename K, typename T>
    using Map = std::unordered_map<K, T>;

    template <typename T>
    using Set = std::unordered_set<T>;

    struct PairHash
    {
        template <typename T, typename U>
        std::size_t operator()(const std::pair<T, U> &p) const
        {
            auto h1 = std::hash<T>{}(p.first);
            auto h2 = std::hash<U>{}(p.second);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };
    
    enum class MouseCellEventType{
        MouseLeaveCell,
        MouseEnterCell,
        
    };

    enum class CellEventType{
        CellAsTarget,
        CellEntered,
        CellLeft,
    };
    enum class MovableEventType
    {
        StatePicked,
        StateUnpicked,
        StateStartMoving,
        StateStopMoving,
        StaeEnterCell,
        StateLeaveCell,
    };
    enum class BuildingType
    {
        House,
        Farm,
        Barrack,
        Market,
        Tower,
        H0085,
    };
    enum class BuildingEventType
    {
        StatePicked,
        StateUnpicked,
    };

    enum class InventoryType
    {
        BuildingPermit,
        Population,
        Food,
        Building,
        LandScape,
        WaterSource,
        FarmLand,

    };

    const std::unordered_map<InventoryType, std::string> InventoryTypeToString = {
        {InventoryType::BuildingPermit, "BuildingPermit"},
        {InventoryType::Population, "Population"},
        {InventoryType::Food, "Food"},
        {InventoryType::Building, "Building"},
        {InventoryType::LandScape, "LandScape"},  
        {InventoryType::WaterSource, "WaterSource"},  
              
    };


};
