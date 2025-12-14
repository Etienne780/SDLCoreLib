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

    SDLCore::UI::UIPropertyRegistry::RegisterBaseProperties();
}

void App::OnUpdate() {

    {
        static int test = 0;

        using namespace SDLCore;
        namespace Prop = SDLCore::UI::Properties;
        UI::SetContextWindow(context, m_winID);
        UI::BindContext(context);

        UI::UIStyle style("Test-Style");
        style.SetValue(Prop::alignVertical, UI::UIAlignment::START);

        if (test == 3) {
            Log::Debug("New Element middle test");
        }
        else if (test == 5) {
            Log::Debug("Delete Element");
        }

        UI::BeginFrame(UI::UIKey("root"));
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
                UI::BeginFrame(UI::UIKey("new element"));
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