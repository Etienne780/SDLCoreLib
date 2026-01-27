#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <CoreLib/Log.h>
#include <CoreLib/Algorithm.h>

#include <CoreLib/Profiler.h>

#include "Types/Audio/SoundManager.h"
#include "Application.h"

namespace SDLCore {

    static Application* m_application = nullptr;

    Application::Application(std::string& name, const Version& version)
        : m_name(name), m_version(version) {
        Init();
        m_application = this;
    }
        
    Application::Application(std::string&& name, const Version& version)
        : m_name(std::move(name)), m_version(version) {
        Init();
        m_application = this;
    }

    Application* Application::GetInstance() {
#ifndef NDEBUG
        if (!m_application) {
            Log::Error("SDLCore::Application::GetInstance: called without an exesting instance!");
        }
#endif
        return m_application;
    }

    bool Application::IsQuit() {
        if (m_application) {
            return m_application->m_closeApplication;
        }
        return true;
    }

    void Application::Init() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            SetError(Log::GetFormattedString("SDLCore::Application: {}", SDL_GetError()));
            cancelStart = 1;
        }
        SDL_SetAppMetadata(m_name.c_str(), m_version.ToString().c_str(), nullptr);

        if (!TTF_Init()) {
            SetError(Log::GetFormattedString("SDLCore::Application: {}", SDL_GetError()));
            cancelStart = 2;
        }

        if (!MIX_Init()) {
            SetError(Log::GetFormattedString("SDLCore::Application: {}", SDL_GetError()));
            cancelStart = 3;
        }

        if (!SoundManager::Init()) {
            // SetError(GetError()); error is already set
            cancelStart = 4;
        }
    }

    int Application::Start() {
        if (cancelStart != 0) {
            MIX_Quit();
            TTF_Quit();
            SDL_Quit();
            return cancelStart;
        }

        uint64_t frameStart = 0;
        OnStart();
        while(!m_closeApplication) {
            Profiler::Begin("app");
            
            frameStart = Time::GetTime();
            Time::Update();

            Profiler::Begin("SDL poll events");
            ProcessSDLPollEvents();
            Profiler::End("SDL poll events");
            if (m_closeApplication)
                break;

            Profiler::Begin("App update");
            OnUpdate();
            Profiler::End("App update");

            Profiler::Begin("Input late update");
            Input::LateUpdate();
            Profiler::End("Input late update");

            Profiler::Begin("Lock cursor");
            LockCursor();
            Profiler::End("Lock cursor");

            Profiler::Begin("FPS cap delay");
            FPSCapDelay(frameStart);
            Profiler::End("FPS cap delay");

            Profiler::End("app");

            auto top = std::chrono::high_resolution_clock::now();
            Profiler::PrintAndReset();
            Log::Print("FPS: {}", SDLCore::Time::GetFrameRate());
            Log::Print("");


            double ms = std::chrono::duration<double, std::milli>(
                std::chrono::high_resolution_clock::now() - top
            ).count();

            Log::Print("print time: {}", ms);
        }
        OnQuit();

        ResetCursorLockParams();
        Render::SetWindowRenderer();
        DeleteAllWindows();

        SoundManager::Quit();
        MIX_Quit();
        TTF_Quit();
        SDL_Quit();

        return 0;
    }

    void Application::Quit() {
        m_closeApplication = true;
    }

    Window* Application::AddWindow(WindowID* idPtr, std::string name, int width, int height) {
        WindowID newID = WindowID(m_windowIDManager.GetNewUniqueIdentifier());
        if (newID.value == SDLCORE_INVALID_ID) {
            Log::Error("SDLCore::Application::AddWindow: Cant add window, id is invalid");
            return nullptr;
        }

        auto& win = m_windows.emplace_back(Window::CreateInstance(newID, name, width, height));
        win->SetVsync(m_vsync);

        if (idPtr) {
            *idPtr = newID;
            win->AddOnDestroy([idPtr]() { idPtr->value = SDLCORE_INVALID_ID; });
        }

        return win.get();
    }

    Window* Application::CreateWindow(WindowID* idPtr, const std::string& name, int width, int height) {
        auto* win = AddWindow(idPtr, name, width, height);
        if (!win)
            return nullptr;

        win->CreateWindow();
        win->CreateRenderer();

        return win;
    }

    bool Application::DeleteWindow(WindowID id) {
        auto it = std::find_if(m_windows.begin(), m_windows.end(),
            [id](const std::unique_ptr<Window>& win) { return win->GetID() == id; });

        if (it == m_windows.end())
            return false;

        (*it)->DestroyWindow();
        m_windows.erase(it);
        return true;
    }

    void Application::DeleteAllWindows() {
        while (!m_windows.empty()) {
            DeleteWindow(m_windows.back()->GetID());
        }
    }

    bool Application::IsCursorLocked() const {
        return !m_cursorLockWinID.IsInvalid();
    }

    bool Application::IsCursorLockCenter() const {
        return !m_cursorLockResizeCallbackID.IsInvalid();
    }

    Window* Application::GetWindow(WindowID id) {
        if (id.value == SDLCORE_INVALID_ID)
            return nullptr;

        return Algorithm::Search::GetLinear<Window>(m_windows, [id](Window& win) -> bool {
            return win.GetID() == id;
        });
    }

    size_t Application::GetWindowCount() const {
        return m_windows.size();
    }


    std::string Application::GetName() const {
        return m_name;
    }

    Version Application::GetVersion() const {
        return m_version;
    }

    WindowID Application::GetCursorLockWinID() const {
        return m_cursorLockWinID;
    }

    void Application::SetFPSCap(int value) {
        if (value < -2)
            value = APPLICATION_FPS_UNCAPPED;
        switch (value)
        {
        case APPLICATION_FPS_VSYNC_ON:
            SetVsyncOnWindows(1);
            m_vsync = -1;
            break;
        case APPLICATION_FPS_VSYNC_ADAPTIVE_ON:
            SetVsyncOnWindows(-1);
            m_vsync = -1;
            break;
        default:
            SetVsyncOnWindows(0);
            m_vsync = 0;
            break;
        }

        m_fpsCap = value;
    }

    bool Application::SetCursorLock(WindowID winID, bool active, bool center) {
        Window* win = GetWindow(winID);
        if (!win) {
            SetErrorF("SDLCore::Application::SetCursorLock: Failed, window with id '{}' dosent exist!", winID);
            return false;
        }

        // invalidate the old resize cb and resets the vars to default values
        ResetCursorLockParams();

        m_cursorLockWinActive = active;
        if (active) {
            // subs to new callback if true
            if (center) {
                m_cursorLockResizeCallbackID = win->AddOnWindowResize(
                    [](Window& win) {
                        Application* app = Application::GetInstance();
                        if (app) {
                            Vector2 size = win.GetSize();
                            app->SetCursorLockPosX(size.x / 2);
                            app->SetCursorLockPosY(size.y / 2);
                        }
                    });
                Vector2 size = win->GetSize();
                m_cursorLockPosX = size.x / 2;
                m_cursorLockPosY = size.y / 2;
            }

            m_cursorLockFocusGainCallbackID = win->AddOnWindowFocusGain(
                [](Window& win) { 
                    auto* app = Application::GetInstance();
                    if (app) {
                        app->SetCursorLockActive(true);
                    }
                });
            m_cursorLockFocusLostCallbackID = win->AddOnWindowFocusLost(
                [](Window& win) { 
                    auto* app = Application::GetInstance();
                    if (app) {
                        app->SetCursorLockActive(false);
                    }
                });

            m_cursorLockLastWinCursorGrab = win->GetCursorGrab();
            win->SetCursorGrab(true);
            m_cursorLockWinID = winID;
            m_cursorLockSDLWin = win->GetSDLWindow();
        }
        return true;
    }

    void Application::SetCursorLockActive(bool active) {
        m_cursorLockWinActive = active;
        if (auto* win = GetWindow(m_cursorLockWinID)) {
            win->SetCursorGrab((active) ? true : m_cursorLockLastWinCursorGrab);
        }
    }

    void Application::SetCursorLockPosX(float x) {
#ifdef DEBUG
        if (IsCursorLockCenter())
            Log::Warn("SDLCore::Application::SetCursorLockPosX: cursorLock is configured to auto-center, manual position update may be ignored");
#endif
        if (x < 0)
            x = 0;
        m_cursorLockPosX = x;
    }

    void Application::SetCursorLockPosY(float y) {
#ifdef DEBUG
        if (IsCursorLockCenter())
            Log::Warn("SDLCore::Application::SetCursorLockPosY: cursorLock is configured to auto-center, manual position update may be ignored");
#endif
        if (y < 0)
            y = 0;
        m_cursorLockPosY = y;
    }

    void Application::SetVsyncOnWindows(int value) {
        for (auto& window : m_windows) {
            window->SetVsync(value);
        }
    }

    void Application::ProcessSDLPollEvents() {
        while (SDL_PollEvent(&m_sdlEvent)) {
            for (auto& window : m_windows) {
                ProcessSDLPollEventWindow(window);
            }
        }

        for (auto& id : m_windowsToClose) {
            auto* win = GetWindow(id);
            if (win)
                win->DestroyWindow();
            DeleteWindow(id);
        }
        m_windowsToClose.clear();
    }

    void Application::ProcessSDLPollEventWindow(const std::unique_ptr<Window>& window) {
        SDL_WindowID sdlWindowID = window->GetSDLID();
        if (m_sdlEvent.window.windowID != sdlWindowID || sdlWindowID == SDLCORE_INVALID_ID)
            return;

        switch (m_sdlEvent.type) {
        case SDL_EVENT_QUIT:
            Quit();
            break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            m_windowsToClose.push_back(window->GetID());
            break;
        case SDL_EVENT_WINDOW_MOVED:
        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_MINIMIZED:
        case SDL_EVENT_WINDOW_MAXIMIZED:
        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_SHOWN:
        case SDL_EVENT_WINDOW_HIDDEN:
        case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
        case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        case SDL_EVENT_WINDOW_FOCUS_LOST:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
        case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
            window->UpdateWindowEvents(m_sdlEvent.type);
            break;
        default:
            break;
        }

        Input::ProcessEvent(m_sdlEvent);
    }

    void Application::FPSCapDelay(uint64_t frameStartTime) const {
        if (m_fpsCap <= 0 || m_vsync != 0)
            return;

        uint64_t targetFrameTime = 1000 / m_fpsCap;
        uint64_t frameTime = SDL_GetTicks() - frameStartTime;

        if (frameTime < targetFrameTime) {
            SDL_Delay(static_cast<Uint32>(targetFrameTime - frameTime));
        }
    }

    void Application::LockCursor() {
        if (!m_cursorLockWinActive)
            return;

        if (Time::GetTimeSec() < m_cursorLastTime + m_cursorTick)
            return;
        m_cursorLastTime = Time::GetTimeSec();

        if (!m_cursorLockWinID.IsInvalid()) {
            if(auto win = m_cursorLockSDLWin.lock())
                SDL_WarpMouseInWindow(win.get(), m_cursorLockPosX, m_cursorLockPosY);
        }
        else {
            ResetCursorLockParams();
        }
    }

    void Application::ResetCursorLockParams() {
        if (auto* win = GetWindow(m_cursorLockWinID)) {
            win->RemoveOnWindowResize(m_cursorLockResizeCallbackID);
            win->RemoveOnWindowFocusGain(m_cursorLockFocusGainCallbackID);
            win->RemoveOnWindowFocusLost(m_cursorLockFocusLostCallbackID);

            win->SetCursorGrab(false);
        }
        
        m_cursorLockWinID.SetInvalid();
        m_cursorLockResizeCallbackID.SetInvalid();
        m_cursorLockFocusGainCallbackID.SetInvalid();
        m_cursorLockFocusLostCallbackID.SetInvalid();
        m_cursorLockSDLWin.reset();
    }

}