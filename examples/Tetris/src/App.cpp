#include "App.h"
#include <SDLCoreLib/SDLCoreUI.h>

App::App()
    : Application("Tetris", SDLCore::Version(1, 0)) {
}

void App::OnStart() {
    SDLCore::Texture tex("J:/images/image.png");
    auto* win = CreateWindow(&m_winID, "Tetris", 800, 800);
    win->SetIcon(tex);


    {
        using namespace SDLCore;

        UI::UIStyle style("Test");
        style.SetSize(10, 10);

        UI::FrameScope root;
        {
            UI::FrameScope(style);
            {
                
            }
        }
    }
}

void App::OnUpdate() {
    // Quit if no windows remain
    if (GetWindowCount() <= 0)
        Quit();

    if (!m_winID.IsInvalid()) {
        using namespace SDLCore;
        Input::SetWindow(m_winID);

        namespace RE = SDLCore::Render;
        RE::SetWindowRenderer(m_winID);
        RE::SetColor(0);
        RE::Clear();

        RE::Present();

        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            DeleteWindow(m_winID);

        if (Time::GetFrameCount() % 200 == 0)
            Log::Print("FPS: {}", Time::GetFrameRateHzF());
    }
}

void App::OnQuit() {
}