#include <SDL3/SDL.h>
#include <CoreLib/Log.h>

#include "Application.h"

namespace SDLCore {

    Application::Application(std::string& name, const Version& version)
        : m_name(name), m_version(version) {
        Init();
    }
        
    Application::Application(std::string&& name, const Version& version)
        : m_name(std::move(name)), m_version(version) {
        Init();
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
            OnUpdate();
        }
        OnQuit();

        m_windows.clear();
        SDL_Quit();

        return 0;
    }

    WindowID Application::AddWindow() {
        WindowID newID = WindowID(m_windowIDManager.GetNewUniqueIdentifier());
        if (newID.value == SDLCORE_INVALID_ID) {
            Log::Error("Application::AddWindow: Cant add window, id is invalid");
           return WindowID{ SDLCORE_INVALID_ID };
        }

        m_windows.push_back(std::make_unique<Window>(newID));
        return newID;
    }

    WindowID Application::CreateWindow(std::string name, int width, int height) {
        WindowID newID = WindowID(m_windowIDManager.GetNewUniqueIdentifier());
        if (newID.value == SDLCORE_INVALID_ID) {
            Log::Error("Application::CreateWindow: Cant add window, id is invalid");
            return WindowID{ SDLCORE_INVALID_ID };
        }

        m_windows.push_back(std::make_unique<Window>(newID, name, width, height));
        return newID;
    }

}