#include "App.h"
#include <SDLCoreLib/SDLCoreUI.h>

App::App()
    : Application("Tetris", SDLCore::Version(1, 0)) {
}

SDLCore::UI::UINumberID spacing_xs;
SDLCore::UI::UIContext* context = SDLCore::UI::CreateContext();
void App::OnStart() {
    SDLCore::Texture tex("J:/images/image.png");
    auto* win = CreateWindow(&m_winID, "Tetris", 800, 800);
    win->SetIcon(tex);

    using namespace SDLCore;
    spacing_xs = UI::UIRegistry::RegisterNumber(16);
}

void App::OnUpdate() {

    if (!m_winID.IsInvalid()) {
        namespace RE = SDLCore::Render;
        SDLCore::Input::SetWindow(m_winID);

        RE::SetWindowRenderer(m_winID);
        RE::SetBlendMode(SDLCore::Render::BlendMode::BLEND);
        RE::SetColor(0);
        RE::Clear();

        using namespace SDLCore;
        namespace Prop = SDLCore::UI::Properties;
        UI::SetContextWindow(context, m_winID);
        UI::BindContext(context);

        UI::UIStyle styleRoot("root");
        styleRoot.SetValue(Prop::backgroundColor, Vector4(0, 0, 0, 0))
            .SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
            .SetValue(Prop::alignHorizontal, UI::UIAlignment::CENTER)
            .SetValue(Prop::alignVertical, UI::UIAlignment::CENTER)
            .SetValue(Prop::sizeUnitW, UI::UISizeUnit::PERCENTAGE)
            .SetValue(Prop::sizeUnitH, UI::UISizeUnit::PERCENTAGE)
            .SetValue(Prop::width, 100.0f)
            .SetValue(Prop::height, 100.0f);

        UI::UIStyle elementStyle("element");
        elementStyle.SetValue(Prop::width, 200.0f).SetValue(Prop::height, 200.0f)
            .SetValue(Prop::borderWidth, 12.0f)
            .SetValue(Prop::borderColor, Vector4(255, 0, 0, 255));
        elementStyle.SetActiveState(UI::UIState::HOVER)
            .SetValue(Prop::borderInset, true);

        UI::BeginFrame(UI::UIKey("root"), styleRoot);
        {
            UI::BeginFrame(UI::UIKey("element"), elementStyle);
            {

            }
            UI::EndFrame();
        }
        UI::EndFrame();

        // Log::Print(UI::GetContextStringHierarchy(context));
        RE::Present();
    }

    /*
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
    }*/
}

void App::OnQuit() {
}