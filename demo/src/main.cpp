
#include "fg/demo/DiamondSquareMapGenerator.h"
// 示例：生成 65x65 高度图并打印部分值
int main()
{
    auto heightmap = DiamondSquare::generate(65, 0.45f, 12345);

    // 归一化到 [0, 1]（可选）
    float min_val = heightmap[0][0], max_val = heightmap[0][0];
    for (const auto &row : heightmap)
    {
        for (float h : row)
        {
            if (h < min_val)
                min_val = h;
            if (h > max_val)
                max_val = h;
        }
    }
    float range = max_val - min_val;
    if (range == 0)
        range = 1;

    // 打印归一化后的前10行前10列（用于调试）
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            float norm = (heightmap[i][j] - min_val) / range;
            printf("%.2f ", norm);
        }
        std::cout << "\n";
    }

    return 0;
}