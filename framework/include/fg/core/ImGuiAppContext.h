#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <Bites/OgreWindowEventUtilities.h>

#include <fmt/format.h>
#include "DispatchInputListener.h"
#include "ImGuiInputListener.h"
#include "ImGuiFPSCounter.h"
#include "fg/ImGuiApp.h"
#include "ImGuiAppImpl.h"

#define MAO_IMGUI_BUTTON_LEFT 0
#define MAO_IMGUI_BUTTON_RIGHT 1
#define MAO_IMGUI_BUTTON_OTHER 2

using namespace Ogre;
using namespace OgreBites;

class ImGuiAppContext : public ApplicationContextSDL
{
protected:
    ImGuiAppImpl *imGuiApp = nullptr;

public:
    ImGuiAppContext(std::string name) : ApplicationContextSDL(name)
    {

        //
    }
    // ========== 清理 ==========
    virtual ~ImGuiAppContext() override
    {
        if (this->imGuiApp)
            delete this->imGuiApp;
    }

    void initApp()
    {
        ApplicationContextSDL::initApp();

        if (mWindows.empty())
        {
            throw new std::runtime_error("no window created?");
        }
        NativeWindowPair window = mWindows[0];
        this->imGuiApp = new ImGuiAppImpl(window);
        imGuiApp->initApp();

        // not this->addInputListener() ,us the super class to add the only one entry point listener.
        ApplicationContextSDL::addInputListener(window.native, imGuiApp);
    }
    ImGuiApp *getImGuiApp()
    {
        return this->imGuiApp;
    }
    bool frameRenderingQueued(const FrameEvent &evt) override
    {

        if (!ApplicationContextBase::frameRenderingQueued(evt))
        {
            return false; // false:break,true:continue
        } // call other listener if any.

        return imGuiApp->frameRenderingQueued(evt);
    }

    // To listen a event, use the ImGuiApp for adding listener. We deliver all input event to the imGuiApp.
    void addInputListener(NativeWindowType *window, InputListener *listener) override
    {
        // to take over the dispathing task,so do not add to the entry listener list,
        // only the dispatcher listener can be there.
        this->imGuiApp->addInputListener(listener);
    }
    void windowResized(Ogre::RenderWindow* rw) {
        ApplicationContextSDL::windowResized(rw);
        this->imGuiApp->windowResized(rw);
    }
};
