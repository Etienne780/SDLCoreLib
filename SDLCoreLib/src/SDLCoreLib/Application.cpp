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
    }

    int Application::Start() {

        OnStart();

        while(!m_closeApplication) {
            OnUpdate();
        }

        OnQuit();

        return 0;
    }

}