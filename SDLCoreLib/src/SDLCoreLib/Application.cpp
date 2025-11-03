#include <SDL3/SDL.h>
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
        if (m_application) {
            Log::Error("Application::GetInstance: called without an exesting instance!");
        }
#endif
        return m_application;
    }

    void Application::Init() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            m_sdlErrorMsg = SDL_GetError();
            cancelStart = 1;
        }
    }

    int Application::Start() {
        if (cancelStart != 0) {
            Log::Error("Application {} Could not start SDL error = {}", m_name, m_sdlErrorMsg);
            return cancelStart;
        }

        OnStart();
        while(!m_closeApplication) {
            ProcessSDLPollEvents();
            if (m_closeApplication)
                break;

            OnUpdate();
        }
        OnQuit();

        m_windows.clear();
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

        for (auto id : m_windowsToClose) {
            auto win = GetWindow(id);
            if (win) 
                win->Destroy();
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

        case SDL_EVENT_KEY_DOWN:
            Log::Print("Key pressed in window '{}'", window->GetName());
            break;

        default:
            break;
        }
    }

    WindowID Application::AddWindow() {
        WindowID newID = WindowID(m_windowIDManager.GetNewUniqueIdentifier());
        if (newID.value == SDLCORE_INVALID_ID) {
            Log::Error("Application::AddWindow: Cant add window, id is invalid");
            return WindowID{ SDLCORE_INVALID_ID };
        }

        m_windows.push_back(Window::CreateInstance(newID));
        return newID;
    }

    WindowID Application::CreateWindow(std::string name, int width, int height) {
        WindowID newID = WindowID(m_windowIDManager.GetNewUniqueIdentifier());
        if (newID.value == SDLCORE_INVALID_ID) {
            Log::Error("Application::CreateWindow: Cant add window, id is invalid");
            return WindowID{ SDLCORE_INVALID_ID };
        }

        auto& win = m_windows.emplace_back(Window::CreateInstance(newID, name, width, height));
        win->Create();
        return newID;
    }

    bool Application::RemoveWindow(WindowID id) {
        auto it = std::remove_if(m_windows.begin(), m_windows.end(),
            [id](const std::unique_ptr<Window>& win) {
                return win->GetID() == id;
            });

        if (it != m_windows.end()) {
            m_windows.erase(it, m_windows.end());
            return true;
        }
        return false;
    }


    Window* Application::GetWindow(WindowID id) {
        return Algorithm::Search::GetLinear<Window>(m_windows, [id](Window& win) {
            return win.GetID() == id;
        });
    }

    size_t Application::GetWindowCount() const {
        return m_windows.size();
    }

}