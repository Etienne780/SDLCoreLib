#include "App.h"

App::App()
    : Application("Wordle", SDLCore::Version(1, 0)) {
}

void App::OnStart() {
    SetupUI();

    SDLCore::Texture tex("J:/images/image.png");
    auto* win = CreateWindow(&m_winID, "Wordle", 800, 800);
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

void App::SetupUI() {
    using namespace SDLCore;
    namespace Prop = SDLCore::UI::Properties;

    context = SDLCore::UI::CreateContext();

    styleRoot.SetValue(Prop::backgroundColor, Vector4(0, 0, 0, 0))
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
        .SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
        .SetValue(Prop::sizeUnit, UI::UISizeUnit::PERCENTAGE, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::size, 100.0f, 100.0f);
}

void App::RenderUI(SDLCore::WindowID winID) {
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