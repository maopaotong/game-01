#pragma once
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <Ogre.h>

namespace fog
{
    typedef Ogre::Vector3 Vector3;
    typedef Ogre::Vector2 Vector2;

    template <typename T>
    using List = std::vector<T>;

    template <typename K, typename T>
    using Map = std::unordered_map<K, T>;

    template <typename T>
    using Set = std::unordered_set<T>;

};
