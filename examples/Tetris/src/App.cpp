#include <cmath>
#include <vector>
#include <array>
#include <CoreLib/Math/Vector4.h>
#include "App.h"

App::App()
    : Application("Tetris", SDLCore::Version(1, 0)) {
}

SDLCore::Window* win = nullptr;
void App::OnStart() {
    win = CreateWindow(&m_winID, "Tetris", 800, 800);
}

void App::OnUpdate() {
    // Quit if no windows remain
    if (GetWindowCount() <= 0)
        Quit();

    if (m_winID != SDLCORE_INVALID_ID) {
        using namespace SDLCore;
        Input::SetWindow(m_winID);

        // Close window with ESC
        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            DeleteWindow(m_winID);

        // 1 = Normal
        if (Input::KeyJustPressed(KeyCode::NUM_1)) {
            // Return to normal windowed mode
            win->SetState(WindowState::NORMAL);
        }

        // 2 = Minimized
        if (Input::KeyJustPressed(KeyCode::NUM_2)) {
            win->SetState(WindowState::MINIMIZED);
        }

        // 3 = Maximized
        if (Input::KeyJustPressed(KeyCode::NUM_3)) {
            win->SetState(WindowState::MAXIMIZED);
        }

        // 4 = Borderless fullscreen
        if (Input::KeyJustPressed(KeyCode::NUM_4)) {
            win->SetState(WindowState::FULLSCREEN_BORDERLESS);
        }

        // 5 = Exclusive fullscreen
        if (Input::KeyJustPressed(KeyCode::NUM_5)) {
            win->SetState(WindowState::FULLSCREEN_EXCLUSIVE);
        }

        // 6 = Hide window
        if (Input::KeyJustPressed(KeyCode::NUM_6)) {
            win->Hide();
        }

        // 7 = Show window again (if hidden)
        if (Input::KeyJustPressed(KeyCode::NUM_7)) {
            win->Show();
        }

        namespace RE = SDLCore::Render;
        RE::SetWindowRenderer(m_winID);
        RE::SetColor(0);
        RE::Clear();

        // Display current state in the window (for debugging)
        RE::SetColor(255);
        RE::SetFontSize(24);
        std::string state = FormatUtils::formatString("State: {}, Visibile {}, Focus {}", win->GetState(), win->IsVisible(), win->IsFocused());
        RE::Text(state, 10, 10);

        RE::Present();
    }
}

void App::OnQuit() {
}