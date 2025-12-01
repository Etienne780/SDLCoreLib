#include <cmath>
#include <vector>
#include <array>
#include <CoreLib/Math/Vector4.h>
#include "App.h"

App::App()
    : Application("Tetris", SDLCore::Version(1, 0)) {
}

void App::OnStart() {
    SDLCore::Window* win = CreateWindow(&m_winID, "Tetris", 800, 800);
    win->SetState(SDLCore::WindowState::FULLSCREEN_BORDERLESS);
}

void App::OnUpdate() {
    if (GetWindowCount() >= 0)
        Quit();

    if (m_winID != SDLCORE_INVALID_ID) {
        namespace RE = SDLCore::Render;
        RE::SetColor(0);
        RE::Clear();
        RE::Present();

        using namespace SDLCore;
        Input::SetWindow(m_winID);
        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            DeleteWindow(m_winID);
    }
}

void App::OnQuit() {
}