#include "App.h"
#include <SDLCoreLib/SDLCoreUI.h>

App::App()
    : Application("Tetris", SDLCore::Version(1, 0)) {
}

SDLCore::UI::UINumberID spacingXS;
SDLCore::UI::UITextureID testImageID;
SDLCore::UI::UIContext* context = SDLCore::UI::CreateContext();

SDLCore::UI::UIStyle styleRoot("root");
SDLCore::UI::UIStyle buttenStyle("element");
SDLCore::UI::UIStyle textStyle("text");

void App::OnStart() {
    SDLCore::Texture tex("J:/images/image.png");
    auto* win = CreateWindow(&m_winID, "Tetris", 800, 800);
    win->SetIcon(tex);

    using namespace SDLCore;
    spacingXS = UI::UIRegistry::RegisterNumber(16);
    testImageID = UI::UIRegistry::RegisterTexture("image.trust");

    namespace Prop = SDLCore::UI::Properties;
    styleRoot.SetValue(Prop::backgroundColor, Vector4(0, 0, 0, 0)).SetImportant(true)
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::START)
        .SetValue(Prop::alignVertical, UI::UIAlignment::CENTER)
        .SetValue(Prop::widthUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::heightUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::width, 100.0f)
        .SetValue(Prop::height, 100.0f);

    buttenStyle.SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        // .SetValue(Prop::propagateStateToChildren, true)
        .SetValue(Prop::backgroundColor, Vector4(255.0f))
        .SetValue(Prop::duration, 1.0f)
        .SetValue(Prop::width, 200.0f).SetValue(Prop::height, 200.0f)
        .SetValue(Prop::borderWidth, 12.0f)
        .SetValue(Prop::borderColor, Vector4(255, 0, 0, 255))
        .SetValue(Prop::backgroundTexture, testImageID);
    buttenStyle.SetActiveState(UI::UIState::HOVER)
        .SetValue(Prop::backgroundColor, Vector4(150.0f, 0.0f, 255.0f, 255.0f))
        .SetValue(Prop::borderColor, Vector4(255, 255, 0, 255))
        .SetValue(Prop::duration, 2.0f);

    textStyle.SetValue(Prop::pointerEvents, true)
        .SetValue(Prop::hitTestTransparent, true)
        .SetValue(Prop::duration, 0.2f)
        .SetValue(Prop::textSize, 64.0f)
        .SetValue(Prop::textColor, Vector4(0, 0, 255, 255));
    textStyle.SetActiveState(UI::UIState::HOVER)
        .SetValue(Prop::textColor, Vector4(255, 100, 100, 255));
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

        UI::BeginFrame(UI::UIKey("root"), styleRoot);
        {
            UI::BeginFrame(UI::UIKey("butten"), buttenStyle)->SetOverride(Prop::width, 500.0f);
            {
                UI::Text(UI::UIKey("text"), "text", textStyle);
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
