#pragma once
#include "fg/Config.h"
namespace fog
{

    float Config::HEIGHT_SCALE = 100;
    int Config::TILES_WIDTH = 128 + 1;
    int Config::TILE_TERRAIN_QUALITY = 4;
    int Config::TILE_MESH_QUALITY = 2;
    float Config::CELL_SCALE = 30.0f;
    float Config::WORLD_WIDTH = CELL_SCALE * 2.0 * TILES_WIDTH;
    float Config::WORLD_HEIGHT = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378
    void Config::init(std::string file)
    {
        Options opts;
        load(file, opts);
        HEIGHT_SCALE = Options::get<float>(opts, "HEIGHT_SCALE", HEIGHT_SCALE);
        TILES_WIDTH = Options::get<int>(opts, "TILES_WIDTH", TILES_WIDTH);
        TILE_TERRAIN_QUALITY = Options::get<int>(opts, "TILE_TERRAIN_QUALITY", TILE_TERRAIN_QUALITY);
        TILE_MESH_QUALITY  = Options::get<int>(opts, "TILE_MESH_QUALITY", TILE_MESH_QUALITY);
        CELL_SCALE = Options::get<float>(opts, "CELL_SCALE", CELL_SCALE);

        WORLD_WIDTH = CELL_SCALE * 2.0 * TILES_WIDTH;
        WORLD_HEIGHT = WORLD_WIDTH * 1.73205080757 /*std::sqrt(3)*/ / 2.0; // 0.86602540378
    }

    void Config::load(std::string file, Options &opts)
    {
        std::ifstream f(file);
        std::string line;
        int lNum = 0;
        while (std::getline(f, line))
        {
            lNum++;

            if (line.empty() || line[0] == '#' || line[0] == '/' && line.length() > 1 && line[1] == '/')
            {
                continue;
            }
            auto eq = line.find('=');
            if (eq == std::string::npos)
            {
                // ignore
                continue;
            }
            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            auto typeLeft = key.find("<");
            std::string type = "string";
            if (typeLeft != std::string::npos)
            {

                auto typeRight = key.find(">");

                if (typeRight == std::string::npos || typeRight != key.length() - 1)
                {
                    throw std::runtime_error(std::string("config format error for lineNum:") + std::to_string(lNum));
                }

                type = key.substr(typeLeft + 1, typeRight - typeLeft - 1);
                key = key.substr(0, typeLeft);
            }

            bool ok;
            if (type == "string")
            {
                ok = opts.tryAdd<std::string>(key, value);
            }
            else if (type == "float")
            {
                ok = opts.tryAdd<float>(key, std::stof(value));
            }
            else if (type == "int")
            {
                ok = opts.tryAdd<int>(key, std::stoi(value));
            }
            else if (type == "bool")
            {
                ok = opts.tryAdd<bool>(key, (value == "true" || value == "yes" || value == "Y"));
            }
            else
            {
                throw std::runtime_error(std::string("not supported type:") + type);
            }
            if (!ok)
            {
                throw std::runtime_error(std::string("config key already exists."));
            }
        }
    }
};