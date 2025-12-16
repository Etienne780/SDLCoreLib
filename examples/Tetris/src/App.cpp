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

    SDLCore::Render::SetBlendMode(SDLCore::Render::BlendMode::BLEND);
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
        styleRoot.SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW);
        styleRoot.SetValue(Prop::alignHorizontal, UI::UIAlignment::CENTER);
        styleRoot.SetValue(Prop::alignVertical, UI::UIAlignment::CENTER);
        styleRoot.SetValue(Prop::sizeUnitW, UI::UISizeUnit::PERCENTAGE);
        styleRoot.SetValue(Prop::sizeUnitH, UI::UISizeUnit::PERCENTAGE);
        styleRoot.SetValue(Prop::width, 100.0f);
        styleRoot.SetValue(Prop::height, 100.0f);

        UI::UIStyle red;
        red.SetValue(Prop::backgroundColor, Vector4(255, 0, 0, 100));

        UI::UIStyle blue;
        blue.SetValue(Prop::backgroundColor, Vector4(255, 0, 0, 255));

        UI::UIStyle green;
        green.SetValue(Prop::backgroundColor, Vector4(255, 0, 0, 255));

        UI::UIStyle px200;
        px200.SetValue(Prop::width, 100.0f).SetValue(Prop::height, 100.0f);

        if (test == 3) {
            Log::Debug("New Element middle test");
        }
        else if (test == 5) {
            Log::Debug("Delete Element");
        }

        UI::BeginFrame(UI::UIKey("root"), styleRoot, red);
        {
            UI::BeginFrame(UI::UIKey("1"), px200, blue);
            UI::EndFrame();

            UI::BeginFrame(UI::UIKey("2"), px200, green);
            UI::EndFrame();
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