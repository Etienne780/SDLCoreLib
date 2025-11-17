#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <CoreLib/Log.h>
#include <CoreLib/Algorithm.h>

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
            m_cancelErrorMsg = SDL_GetError();
            cancelStart = 1;
        }

        if (!TTF_Init()) {
            m_cancelErrorMsg = SDL_GetError();
            cancelStart = 2;
        }
    }

    int Application::Start() {
        if (cancelStart != 0) {
            Log::Error("SDLCore::Application::Start: {} Could not start SDL error = {}", m_name, m_cancelErrorMsg);
            return cancelStart;
        }

        uint64_t frameStart = 0;
        OnStart();
        while(!m_closeApplication) {
            frameStart = Time::GetTime();
            Time::Update();

            ProcessSDLPollEvents();
            if (m_closeApplication)
                break;

            OnUpdate();
            Input::LateUpdate();
            FPSCapDelay(frameStart);
        }
        OnQuit();

        Renderer::SetWindowRenderer();
        m_windows.clear();
        TTF_Quit();
        SDL_Quit();

        return 0;
    }

    void Application::Quit() {
        m_closeApplication = true;
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
            RemoveWindow(id);
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

        default:
            break;
        }

        // update input
        Input::ProcessEvent(m_sdlEvent);
    }

    void Application::FPSCapDelay(uint64_t frameStartTime) {
        if (m_fpsCap <= 0 || m_vsync != 0)
            return;

        uint64_t targetFrameTime = 1000 / m_fpsCap;
        uint64_t frameTime = SDL_GetTicks() - frameStartTime;

        if (frameTime < targetFrameTime) {
            SDL_Delay(static_cast<Uint32>(targetFrameTime - frameTime));
        }
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
            win->AddOnClose([idPtr]() { idPtr->value = SDLCORE_INVALID_ID; });
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

    bool Application::RemoveWindow(WindowID id) {
        auto it = std::find_if(m_windows.begin(), m_windows.end(),
            [id](const std::unique_ptr<Window>& win) { return win->GetID() == id; });

        if (it == m_windows.end())
            return false;

        (*it)->DestroyWindow();
        m_windows.erase(it);
        return true;
    }


    Window* Application::GetWindow(WindowID id) {
        if (id.value == SDLCORE_INVALID_ID)
            return nullptr;

        return Algorithm::Search::GetLinear<Window>(m_windows, [id](Window& win) {
            return win.GetID() == id;
        });
    }

    size_t Application::GetWindowCount() const {
        return m_windows.size();
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

    void Application::SetVsyncOnWindows(int value) {
        for (auto& window : m_windows) {
            window->SetVsync(value);
        }
    }

}