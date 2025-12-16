#include "App.h"
#include <SDLCoreLib/SDLCoreUI.h>

App::App()
    : Application("Tetris", SDLCore::Version(1, 0)) {
}

SDLCore::UI::UIContext* context = SDLCore::UI::CreateContext();
void App::OnStart() {
    SDLCore::Texture tex("J:/images/image.png");
    auto* win = CreateWindow(&m_winID, "Tetris", 800, 800);
    win->SetIcon(tex);
}

void App::OnUpdate() {

    {
        namespace RE = SDLCore::Render;
        RE::SetWindowRenderer(m_winID);
        RE::SetColor(0);
        RE::Clear();

        static int test = 0;

        using namespace SDLCore;
        namespace Prop = SDLCore::UI::Properties;
        UI::SetContextWindow(context, m_winID);
        UI::BindContext(context);

        UI::UIStyle styleRoot("root");
        styleRoot.SetValue(Prop::sizeUnitW, UI::UISizeUnit::PERCENTAGE);
        styleRoot.SetValue(Prop::sizeUnitH, UI::UISizeUnit::PERCENTAGE);
        styleRoot.SetValue(Prop::width, 100.0f);
        styleRoot.SetValue(Prop::height, 100.0f);

        UI::UIStyle style("Test-Style");
        style.SetValue(Prop::alignVertical, UI::UIAlignment::START);
        style.SetValue(Prop::sizeUnitW, UI::UISizeUnit::PX);
        style.SetValue(Prop::sizeUnitH, UI::UISizeUnit::PX);
        style.SetValue(Prop::width, 200.0f);
        style.SetValue(Prop::height, 200.0f);

        if (test == 3) {
            Log::Debug("New Element middle test");
        }
        else if (test == 5) {
            Log::Debug("Delete Element");
        }

        UI::BeginFrame(UI::UIKey("root"), styleRoot);
        {
            UI::BeginFrame(UI::UIKey("panel_1"), style);
            {
                UI::BeginFrame(UI::UIKey("panel_1_1"), style);
                {
                    UI::Text(UI::UIKey("text"), "cool");
                }
                UI::EndFrame();
            }
            UI::EndFrame();

            if (test >= 3) {
                UI::BeginFrame(UI::UIKey("new element"), style);
                UI::EndFrame();
            }

            if (test < 5) {
                UI::BeginFrame(UI::UIKey("panel_2"), style);
                UI::EndFrame();
            }
        }
        UI::EndFrame();

        Log::Print("Test {}", test);
        Log::Print(UI::GetContextStringHierarchy(context));

        test++;

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