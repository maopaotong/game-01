#include <vector>
#include <random>
#include <cmath>
#include <cassert>
#include <iostream>

class DiamondSquare
{
public:
    static std::vector<std::vector<float>> generate(int size, float roughness = 0.5f, unsigned int seed = 0)
    {
        // 检查 size 是否为 2^n + 1
        int n = static_cast<int>(log2(size - 1));
        assert((1 << n) + 1 == size && "Size must be 2^n + 1 (e.g., 65, 129, 257)");

        std::vector<std::vector<float>> heightmap(size, std::vector<float>(size, 0.0f));

        // 初始化随机数生成器
        std::mt19937 rng(seed);
        std::uniform_real_distribution<float> rand(-1.0f, 1.0f);

        // 初始化四个角（可设为 0 或小随机值）
        heightmap[0][0] = rand(rng);
        heightmap[0][size - 1] = rand(rng);
        heightmap[size - 1][0] = rand(rng);
        heightmap[size - 1][size - 1] = rand(rng);

        int step = size - 1; // 当前正方形边长
        float scale = 1.0f;  // 随机扰动幅度

        while (step > 1)
        {
            int half = step / 2;

            // ---------- Diamond Step ----------
            for (int y = 0; y < size - 1; y += step)
            {
                for (int x = 0; x < size - 1; x += step)
                {
                    float avg = (heightmap[y][x] +
                                 heightmap[y][x + step] +
                                 heightmap[y + step][x] +
                                 heightmap[y + step][x + step]) /
                                4.0f;

                    heightmap[y + half][x + half] = avg + rand(rng) * scale;
                }
            }

            // ---------- Square Step ----------
            for (int y = 0; y < size; y += half)
            {
                for (int x = (y + half) % step; x < size; x += step)
                {
                    float total = 0.0f;
                    int count = 0;

                    // 上
                    if (y - half >= 0)
                    {
                        total += heightmap[y - half][x];
                        ++count;
                    }
                    // 下
                    if (y + half < size)
                    {
                        total += heightmap[y + half][x];
                        ++count;
                    }
                    // 左
                    if (x - half >= 0)
                    {
                        total += heightmap[y][x - half];
                        ++count;
                    }
                    // 右
                    if (x + half < size)
                    {
                        total += heightmap[y][x + half];
                        ++count;
                    }

                    if (count > 0)
                    {
                        heightmap[y][x] = total / count + rand(rng) * scale;
                    }
                }
            }

            step /= 2;
            scale *= roughness; // 衰减扰动
        }

        return heightmap;
    }
};
