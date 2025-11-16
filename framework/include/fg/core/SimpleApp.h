#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>

#include "fg/util/HexGridPrinter.h"
#include "fg/CostMapControl.h"
#include "fg/Module.h"
#include "fg/App.h"
#include "fg/core/SimpleCore.h"
#include "fg/Global.h"

namespace fog{

using namespace OgreBites;
using namespace Ogre;
class SimpleApp : public App
{
private:
    Core *core;
    std::vector<Module *> list;
    std::unordered_map<std::string, Module *> map;


public:
    SimpleApp()
    {
        this->core = new SimpleCore();
    }

    void add(Module *mod) override
    {

        std::string name = mod->getName();
        if (map.find(name) != map.end())
        {
            throw std::runtime_error("Module already exists:" + name);
        }
        map[name] = mod;
        list.push_back(mod);
    }

    void setup() override
    {
        for (auto it = list.begin(); it != list.end(); it++)
        {
            Module *mod = *it;
            mod->active(this->core);
        }
    }

    void startRendering() override
    {

        Ogre::Root *root = this->core->getRoot();

        root->startRendering();

    }

    void close() override
    {

        std::cout << "Closing application.\n";
        for (auto it = list.rbegin(); it != list.rend(); it++)
        {
            Module *mod = *it;
            std::cout << "Disactive module:" << mod->getName() << "" << std::endl;
            mod->disactive();
            std::cout << "Done of disactive module." << std::endl;
        }

        ////TODO: context->close() at present time will crash the process.
        // Cause:
        //  After add the terrain code by module of GameTerrain.h.
        // 
        // Root cause:
        //  Unknown.
        //
        // Research:
        //----------------------------------------
        //1. The first error is an Ogre defined exception:
        //1. Since SharderGenerator maintaned types of SubSharderFactory,
        //1. One of the factory is FFPTransform factory, this factory will raise a exception saying the RenderState 
        //1. instance is not empty. It must be empty before destroy the factory.
        //2. The second exception will raise if you fix the first one by destroying this factory's all instance 
        //2. before calling OgreBites::Application context::cose();
        //so we give up closing the context of ogre.
        //core->getAppContext()->closeApp();
        //----------------------------------------
    
    }
};
};//end of namespace