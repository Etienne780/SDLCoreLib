#include "App.h"
#include <SDLCoreLib/SDLCoreUI.h>

App::App()
    : Application("Tetris", SDLCore::Version(1, 0)) {
}

SDLCore::UI::UINumberID spacingXS;
SDLCore::UI::UITextureID testImageID;
SDLCore::UI::UIContext* context = SDLCore::UI::CreateContext();

SDLCore::UI::UIStyle styleRoot("root");
SDLCore::UI::UIStyle elementStyle("element");
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
        .SetValue(Prop::alignHorizontal, UI::UIAlignment::START)
        .SetValue(Prop::alignVertical, UI::UIAlignment::CENTER)
        .SetValue(Prop::widthUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::heightUnit, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::width, 100.0f)
        .SetValue(Prop::height, 100.0f);

    elementStyle.SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::width, 200.0f).SetValue(Prop::height, 200.0f)
        .SetValue(Prop::borderWidth, 12.0f)
        .SetValue(Prop::borderColor, Vector4(255, 0, 0, 255))
        .SetValue(Prop::backgroundTexture, testImageID);
    elementStyle.SetActiveState(UI::UIState::HOVER)
        .SetValue(Prop::borderInset, true);

    textStyle.SetValue(Prop::hitTestEnabled, false)
        .SetValue(Prop::textSize, 64.0f)
        .SetValue(Prop::textColor, Vector4(0, 0, 255, 255));
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

        {
            SDLCore::Debug::ProfilerScope das("UI");
            UI::BeginFrame(UI::UIKey("root"), styleRoot);
            {
                for (int i = 0; i < 500; i++) {
                    UI::BeginFrame(UI::UIKey("element-" + std::to_string(i)), elementStyle);
                    {
                        UI::Text(UI::UIKey("Test"), "Test", textStyle);
                        UI::Text(UI::UIKey("Test1"), "Test", textStyle);
                        UI::Text(UI::UIKey("Test2"), "Test", textStyle);
                        UI::Text(UI::UIKey("Test3"), "Test", textStyle);
                        UI::Text(UI::UIKey("Test4"), "Test", textStyle);
                        UI::Text(UI::UIKey("Test5"), "Test", textStyle);
                    }
                    UI::EndFrame();
                }
            }
            UI::EndFrame();
        }

        // Log::Print(UI::GetContextStringHierarchy(context));
        {
            SDLCore::Debug::ProfilerScope RenderPresent("RenderPresent");
            RE::Present();
        }

        if(SDLCore::Time::GetFrameCount() % 120 == 0)
            SDLCore::Debug::Profiler::PrintAndReset();
        else 
            SDLCore::Debug::Profiler::Reset();
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