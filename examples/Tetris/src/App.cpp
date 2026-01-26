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
SDLCore::UI::UIStyle innerBtnStyle("inner");
SDLCore::UI::UIStyle textStyle("text");

void App::OnStart() {
    SDLCore::Texture tex("J:/images/image.png");
    auto* win = CreateWindow(&m_winID, "Tetris", 800, 800);
    win->SetIcon(tex);

    using namespace SDLCore;
    spacingXS = UI::UIRegistry::RegisterNumber(16);
    testImageID = UI::UIRegistry::RegisterTexture("image.trust");

    namespace Prop = SDLCore::UI::Properties;
    styleRoot.SetValue(Prop::backgroundColor, Vector4(0, 0, 0, 0))
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::CENTER)
        .SetValue(Prop::alignVertical, UI::UIAlignment::CENTER)
        .SetValue(Prop::widthUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::heightUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::width, 100.0f)
        .SetValue(Prop::height, 100.0f);

    buttenStyle.SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
        .SetValue(Prop::backgroundColor, Vector4(255.0f))
        .SetValue(Prop::duration, 0.25f)
        .SetValue(Prop::width, 200.0f).SetValue(Prop::height, 200.0f)
        .SetValue(Prop::borderWidth, 12.0f)
        .SetValue(Prop::borderColor, Vector4(255, 0, 0, 255))
        .SetValue(Prop::backgroundTexture, testImageID);
    buttenStyle.SetActiveState(UI::UIState::HOVER)
        .SetValue(Prop::backgroundColor, Vector4(150.0f, 0.0f, 255.0f, 255.0f))
        .SetValue(Prop::borderColor, Vector4(255, 255, 0, 255));

    innerBtnStyle.SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
        .SetValue(Prop::sizeUnit, UI::UISizeUnit::PX, UI::UISizeUnit::PX)
        .SetValue(Prop::size, 500.0f, 150.0f)
        .SetValue(Prop::backgroundColor, Vector4(255));
    innerBtnStyle.SetActiveState(UI::UIState::HOVER)
        .SetValue(Prop::backgroundColor, Vector4(100, 100, 100, 255));

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

        static bool clipX = false;
        static bool clipY = false;

        if (SDLCore::Input::KeyJustPressed(SDLCore::KeyCode::W))
            clipX = !clipX;

        if (SDLCore::Input::KeyJustPressed(SDLCore::KeyCode::E))
            clipY = !clipY;

        float t = SDLCore::Time::GetFrameCount();
        float w = 0, h = 0;
        w = (std::cos(t * 0.001f) + 1) * 0.5f * 500.0f;
        h = (std::sin(t * 0.001f) + 1) * 0.5f * 500.0f;

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
            UI::BeginFrame(UI::UIKey("butten"), buttenStyle)->SetOverride(Prop::size, w, h)
                .SetOverride(Prop::overflowVisible, !clipX, !clipY);
            {
                UI::BeginFrame(UI::UIKey("inner"), innerBtnStyle);
                {
                    UI::Text(UI::UIKey("text"), "text", textStyle);
                }
                UI::EndFrame();
            }
            UI::EndFrame();
        }
        UI::EndFrame();

        if (SDLCore::Time::GetFrameCount() % 200 == 0)
            Log::Print(SDLCore::Time::GetFrameRate());
        //     Log::Print(UI::GetContextStringHierarchy(context));
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
