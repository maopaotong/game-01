// main.cpp - Complete Ogre A* Hex Grid Visualization System
#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <algorithm>
#include <functional>

#include "fg/game/Game01.h"
#include "fg/ui/OnFrameUI.h"
#include "fg/core/SimpleApp.h"

// === Custom hash function ===
//
using namespace fog;
// === Main function ===
int main()
{
    std::cout << "OGRE Version: "
              << OGRE_VERSION_MAJOR << "."
              << OGRE_VERSION_MINOR << "."
              << OGRE_VERSION_PATCH << std::endl;
    try
    {
        std::cout << "Weighted Hexagonal Grid Navigation System\n";
        std::cout << "=========================================\n\n";

        // Initialize Ogre application context
        Game01 *game01 = new Game01();
        

        auto app = std::make_unique<SimpleApp>();
        app->add(game01);
        app->setup();
        app->startRendering();
        app->close();
    }
    catch (const Ogre::Exception &e)
    {
        std::cerr << "Ogre::Exception: " << e.what() << "\n";
        return 1;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "std::runtime_error: " << e.what() << "\n";        
    }
    catch (const std::exception &e)
    {
        std::cerr << "std::exception: " << e.what() << "\n";
        return 1;
    }
    catch (char *e)
    {
        std::cerr << "char*: " << e << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown Error." << std::endl;
    }

    return 0;
}
