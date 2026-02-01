#include "App.h"
#include <SDLCoreLib/SDLCoreUI.h>

App::App()
    : Application("NAME", SDLCore::Version(1, 0)) {
}

void SetupUI();
void RenderUI(SDLCore::WindowID winID);

SDLCore::UI::UIContext* context = SDLCore::UI::CreateContext();
SDLCore::UI::UIStyle styleRoot("root");

void App::OnStart() {
    SetupUI();

    SDLCore::Texture tex("J:/images/image.png");
    auto* win = CreateWindow(&m_winID, "window", 800, 800);
    win->SetIcon(tex);
}

void App::OnUpdate() {

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
        RE::SetBlendMode(SDLCore::Render::BlendMode::BLEND);
        RE::SetColor(0);
        RE::Clear();

        // =====================
        // Render code here ...
        // =====================

        RenderUI(m_winID);

        RE::Present();

        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            DeleteWindow(m_winID);
    }
}

void App::OnQuit() {
}

void SetupUI() {
    using namespace SDLCore;
    namespace Prop = SDLCore::UI::Properties;
    styleRoot.SetValue(Prop::backgroundColor, Vector4(0, 0, 0, 0))
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::CENTER)
        .SetValue(Prop::alignVertical, UI::UIAlignment::CENTER)
        .SetValue(Prop::widthUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::heightUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::width, 100.0f)
        .SetValue(Prop::height, 100.0f);
}

void RenderUI(SDLCore::WindowID winID) {
    using namespace SDLCore;
    namespace Prop = SDLCore::UI::Properties;
    UI::SetContextWindow(context, winID);
    UI::BindContext(context);

    UI::BeginFrame(UI::UIKey("root"), styleRoot);
    {
        // =====================
        // UI here ...
        // =====================
    }
    UI::EndFrame();
}