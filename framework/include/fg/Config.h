#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include "fg/Options.h"
namespace fog
{
    class Config
    {
    public:
        static float HEIGHT_SCALE;
        static int TILES_WIDTH;
        static int TILE_SUBDIVISION_QUALITY;
        static float CELL_SCALE;
        static float WORLD_WIDTH;  // = CELL_SCALE * 2.0 * TILES_WIDTH;
        static float WORLD_HEIGHT; // = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378

    public:
        static void init(std::string file);

        static void load(std::string file, Options &opts);

    }; // end of class
};