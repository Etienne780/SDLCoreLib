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

    /*
// SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay())
UI::SetWindowScale()

fontNumberKey = UI::RegisterFontSize(fontSize);
UI::ActivateFontSize(fontNumberKey);
UI::SetFontSize();

colorNumberKey = UI::RegisterColor(color);
UI::ActivateBackgroundColor(colorNumberKey);
UI::ActivateBorderColor(colorNumberKey);
UI::ActivateTintColor(colorNumberKey);
UI::ActivateTextColor(colorNumberKey);
UI::SetBackgroundColor(Color);
UI::SetBorderColor(Color);
UI::SetTintColor(Color);
UI::SetTextColor(Color);

imageNumberKey = UI::RegisterImage(image);

UI::SetNextMargin(value, value, value, value)
UI::SetNextPadding(value, value, value, value)

UI::SetNextSize()
UI::SetNextPosition()

UI::SetNextTypeRelative()
- if a ui is static it 0,0 will be the position of this element
    like in css Position: relative

UI::SetNextUIType(static, dynamic)
- static: UI has a static position, like in css Position: absolute
- dynamic: UI has a dynamic position with anchor points

UI::SetNextAnchorPoint(Ver: START/CENTER/END, Hor: START/CENTER/END)

UI::GetPosition()
UI::GetSize()
UI::GetTypeRelative()
UI::GetUIType()
UI::GetAnchorVer()
UI::GetAnchorHor()

UI::Begin(imageNumberKey);
{
    UI::Text("text");
}
UI::End();
*/
}

void App::OnQuit() {
}