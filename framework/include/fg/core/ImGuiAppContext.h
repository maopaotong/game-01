#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <Bites/OgreWindowEventUtilities.h>
#include <imgui.h>
// #include "imgui/imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <fmt/format.h>
#include "DispatchInputListener.h"
#include "ImGuiInputListener.h"

#define MAO_IMGUI_BUTTON_LEFT 0
#define MAO_IMGUI_BUTTON_RIGHT 1
#define MAO_IMGUI_BUTTON_OTHER 2

using namespace Ogre;
using namespace OgreBites;

class ImGuiAppContext : public ApplicationContextSDL, public WindowEventListener
{

protected:
    DispatchInputListener *dispatch;
    ImGuiInputListener *guiListener;

public:
    ImGuiAppContext(std::string name) : ApplicationContextSDL(name)
    {
        this->dispatch = new DispatchInputListener();
        this->guiListener = new ImGuiInputListener();
        this->dispatch->addInputListener(this->guiListener);
    }
    // ========== 清理 ==========
    virtual ~ImGuiAppContext() override
    {
        Ogre::WindowEventUtilities::removeWindowEventListener(getRenderWindow(), this);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
        delete this->dispatch;
        delete this->guiListener;
    }

    void initApp()
    {
        ApplicationContextSDL::initApp();
        // Create world state and controls.
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

        if (mWindows.empty())
        {
            throw new std::runtime_error("no window created?");
        }
        NativeWindowPair window = mWindows[0];
        syncWindowSize(io, window.render);
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // NativeWindowPair wp = mWindows[0];//what about other window?
        // SDL_Window * window = wp.native;
        // // // Setup Platform/Renderer backends
        // 初始化 OpenGL3 后端（OGRE 已激活 OpenGL context）
        ImGui_ImplOpenGL3_Init("#version 130");

        Ogre::WindowEventUtilities::addWindowEventListener(getRenderWindow(), this);
        // 注册自己为输入监听器
        ApplicationContextSDL::addInputListener(window.native, dispatch);
    }

    void addInputListener(NativeWindowType *window, InputListener *listener) override
    {
        // to take over the dispathing task,so do not add to the entry listener list, 
        //only the dispatcher listener can be there.
        dispatch->addInputListener(listener);
    }

    // ========== 渲染循环 ==========
    bool frameRenderingQueued(const FrameEvent &evt) override
    {

        // 开始 ImGui 帧
        /*
         */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // 你的 UI
        static bool show_demo = true;
        ImGui::ShowDemoWindow(&show_demo);
        // 渲染 3D 场景（由 OGRE 自动处理）

        // 渲染 ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        return true;
    }

    //
    void windowResized(Ogre::RenderWindow *rw) override
    {

        if (rw != getRenderWindow())
        {
            return;
        }
        ImGuiIO &io = ImGui::GetIO();
        syncWindowSize(io, rw);
    }
    void syncWindowSize(ImGuiIO &io, Ogre::RenderWindow *window)
    {

        unsigned int width, height;
        int left, top;
        window->getMetrics(width, height, left, top); // 👈 获取当前窗口尺寸

        io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
    }

    // 可选：窗口关闭时清理
    void windowClosed(Ogre::RenderWindow *rw) override
    {
        if (rw == getRenderWindow())
            Ogre::WindowEventUtilities::removeWindowEventListener(rw, this);
    }

    void addInputListener(InputListener *lis)
    {
        dispatch->addInputListener(lis);
    }

protected:
};
