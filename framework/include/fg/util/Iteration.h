
#pragma once
#include <vector>
namespace fog
{
    class Iteration
    {
    public:
        template <typename T, typename F>
        static void forEach(std::vector<std::vector<T>> map, int width, int height, F &&visit)
        {
            for (int i = 0; i < width; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    visit(i, j, map[i][j]);
                }
            }
        }
    };
}; // end of namespace