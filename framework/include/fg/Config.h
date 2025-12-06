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
        static int TILE_TERRAIN_QUALITY;
        static int TILE_MESH_QUALITY;
        static float CELL_SCALE;
        static float WORLD_WIDTH;     // = CELL_SCALE * 2.0 * TILES_WIDTH;
        static float WORLD_HEIGHT;    // = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378
                                      //
        static float HEIGHT_OCEAN;    // = 0.49f * 0.9f;
        static float HEIGHT_SHORE;    // = 0.50f * 0.9f;
        static float HEIGHT_PLAIN;    // = 0.51f * 1.1f;
        static float HEIGHT_HILL;     // = 0.52f * 1.2f;
        static float HEIGHT_MOUNTAIN; // = 0.53f * 1.f;
        static float HEIGHT_FROZEN;   // = 0.54f * 1.f;

    public:
        static void init(std::string file);

        static void load(std::string file, Options &opts);

    }; // end of class
};