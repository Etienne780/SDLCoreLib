#include <cmath>
#include <vector>
#include <array>
#include <CoreLib/Math/Vector4.h>
#include "App.h"

App::App()
    : Application("Tetris", SDLCore::Version(1, 0)) {
}

void App::OnStart() {
    SDLCore::Texture tex("J:/images/image.png");
    auto* win = CreateWindow(&m_winID, "Tetris", 800, 800);

    win->SetIcon(tex);
}

void App::OnUpdate() {
    // Quit if no windows remain
    if (GetWindowCount() <= 0)
        Quit();

    if (m_winID != SDLCORE_INVALID_ID) {
        using namespace SDLCore;
        Input::SetWindow(m_winID);


        namespace RE = SDLCore::Render;
        RE::SetWindowRenderer(m_winID);
        RE::SetColor(0);
        RE::Clear();

        RE::Present();
    }
}

void App::OnQuit() {
}