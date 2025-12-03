
#include "fg/demo/DiamondSquare.h"
// 示例：生成 65x65 高度图并打印部分值
int main()

{

    int size = 513;
    std::vector<std::vector<float>> heightmap(size, std::vector<float>(size, 0.0f));
    fog::DiamondSquare::generateAndNormalise(heightmap, 65, 0.45f, 12345);

    return 0;
}