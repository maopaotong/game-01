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
#include "fg/game/OnFrameUI.h"
#include "fg/core/SimpleApp.h"

// === Custom hash function ===
//

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
        auto app = std::make_unique<SimpleApp>();
        app->add(new Game01());
        app->add(new OnFrameUI());
        app->setup();
        app->startRendering();
        app->close();
    }
    catch(Ogre::Exception& e){
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    catch (const char* &e)
    {
        std::cerr << "Error: " << e << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown Error." << std::endl;
    }

    return 0;
}
