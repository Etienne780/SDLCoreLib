#include "App.h"
#include <CoreLib/Random.h>
#include <CoreLib/OTNFile.h>

App::App()
    : Application("NAME", SDLCore::Version(1, 0)) {
}

void App::OnStart() {
    CreateWindow(&m_winID, "window", 800, 800);
}

void App::OnUpdate() {
    return;

    // Quit if no windows remain
    if (GetWindowCount() <= 0)
        Quit();

    if (!m_winID.IsInvalid()) {
        using namespace SDLCore;
        Input::SetWindow(m_winID);

        // =====================
        // Input/Calculations here ...
        // =====================

        namespace RE = SDLCore::Render;
        RE::SetWindowRenderer(m_winID);
        RE::SetColor(0);
        RE::Clear();

        // =====================
        // Render code here ...
        // =====================

        RE::Present();

        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            DeleteWindow(m_winID);
    }
}

void App::OnQuit() {
}