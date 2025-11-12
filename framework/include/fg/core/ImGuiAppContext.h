#pragma once
#include <OgreApplicationContext.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <Bites/OgreWindowEventUtilities.h>
#include <imgui.h>
// #include "imgui/imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <fmt/format.h>
#define MAO_IMGUI_BUTTON_LEFT 0
#define MAO_IMGUI_BUTTON_RIGHT 1
#define MAO_IMGUI_BUTTON_OTHER 2

using namespace Ogre;
using namespace OgreBites;

class ImGuiAppContext : public ApplicationContextSDL, public InputListener, public WindowEventListener
{
    
protected:
    struct ListenerPair{
        InputListenerList dispatcher;
        NativeWindowType *win;
    };
    
public:
    ImGuiAppContext(std::string name) : ApplicationContextSDL(name)
    {
    }
    // ========== 清理 ==========
    virtual ~ImGuiAppContext() override
    {
        Ogre::WindowEventUtilities::removeWindowEventListener(getRenderWindow(), this);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
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
        
        if(mWindows.empty()){
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
        
        ApplicationContextSDL::addInputListener(window.native, this);

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

    void addInputListener(NativeWindowType *win, InputListener *lis) override
    {
       //take over the listener dispating op here

    }

    // ========== 输入事件转发给 ImGui ==========
    bool keyPressed(const KeyboardEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        if (evt.keysym.sym >= 0 && evt.keysym.sym < 256)
            io.AddKeyEvent(ImGuiKey(evt.keysym.sym), true);
        return io.WantCaptureKeyboard;
    }

    bool keyReleased(const KeyboardEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        if (evt.keysym.sym >= 0 && evt.keysym.sym < 256)
            io.AddKeyEvent(ImGuiKey(evt.keysym.sym), false);
        bool ret = io.WantCaptureKeyboard;

        log(fmt::format("io.WantCapterKeyboard is {}", ret));
        return ret;
    }

    void log(std::string msg)
    {
        std::cout << msg << std::endl;
    }

    bool mouseMoved(const MouseMotionEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        io.AddMousePosEvent((float)evt.x, (float)evt.y);
        return io.WantCaptureMouse;
    }

    bool mousePressed(const MouseButtonEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        int button = evt.button == ButtonType::BUTTON_LEFT ? MAO_IMGUI_BUTTON_LEFT : (evt.button == ButtonType::BUTTON_RIGHT ? MAO_IMGUI_BUTTON_RIGHT : MAO_IMGUI_BUTTON_OTHER);
        io.AddMouseButtonEvent(button, true);
        bool ret = io.WantCaptureMouse;
        log(fmt::format("io.WantCaptureMouse is {}", ret));
        return ret;
    }

    bool mouseReleased(const MouseButtonEvent &evt) override
    {
        ImGuiIO &io = ImGui::GetIO();
        int button = evt.button == ButtonType::BUTTON_LEFT ? MAO_IMGUI_BUTTON_LEFT : (evt.button == ButtonType::BUTTON_RIGHT ? MAO_IMGUI_BUTTON_LEFT : MAO_IMGUI_BUTTON_OTHER);
        io.AddMouseButtonEvent(button, false);

        bool ret = io.WantCaptureMouse;
        log(fmt::format("io.WantCaptureMouse is {}", ret));
        return ret;
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
};
