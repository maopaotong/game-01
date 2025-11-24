#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace fog{
enum MarkType
{    
    ACTIVE
};
using CellKey = std::pair<int, int>;

class CellKeyUtil
{
    public:
    static std::string toString(const CellKey& cKey)
    {
        return "(" + std::to_string(cKey.first) + "," + std::to_string(cKey.second) + ")";
    }
};

};//end of namespace