#pragma once
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <string>
#include <tuple>
#include <memory>

#include <Ogre.h>

namespace fog
{
        
    using Vector2 = Ogre::Vector2;

    using Vector3 = Ogre::Vector3;

    using String = std::string;

    template <typename T>
    using List = std::vector<T>;

    template <typename T>
    using UniquePtr = std::unique_ptr<T>;

    template <typename K, typename T>
    using Map = std::unordered_map<K, T>;

    template <typename T>
    using Set = std::unordered_set<T>;

};
