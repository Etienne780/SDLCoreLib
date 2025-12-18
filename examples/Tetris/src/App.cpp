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
        styleRoot.SetValue(Prop::backgroundColor, Vector4(0, 0, 0, 0));
        styleRoot.SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW);
        styleRoot.SetValue(Prop::alignHorizontal, UI::UIAlignment::CENTER);
        styleRoot.SetValue(Prop::alignVertical, UI::UIAlignment::CENTER);
        styleRoot.SetValue(Prop::sizeUnitW, UI::UISizeUnit::PERCENTAGE);
        styleRoot.SetValue(Prop::sizeUnitH, UI::UISizeUnit::PERCENTAGE);
        styleRoot.SetValue(Prop::width, 100.0f);
        styleRoot.SetValue(Prop::height, 100.0f);

        UI::UIStyle red;
        red.SetValue(Prop::backgroundColor, Vector4(255, 0, 0, 255));
        red.SetActiveState(SDLCore::UI::UIState::HOVER);
        red.SetValue(Prop::backgroundColor, Vector4(155, 0, 0, 255));
        red.SetValue(Prop::width, 230.0f);

        UI::UIStyle green;
        green.SetValue(Prop::backgroundColor, Vector4(0, 255, 0, 255));
        green.SetActiveState(SDLCore::UI::UIState::HOVER);
        green.SetValue(Prop::backgroundColor, Vector4(0, 155, 0, 255));
        green.SetValue(Prop::width, 230.0f);

        UI::UIStyle blue;
        blue.SetValue(Prop::backgroundColor, Vector4(0, 0, 255, 255));
        blue.SetActiveState(SDLCore::UI::UIState::HOVER);
        blue.SetValue(Prop::backgroundColor, Vector4(0, 0, 155, 255));
        blue.SetValue(Prop::width, 230.0f);

        UI::UIStyle yellow;
        yellow.SetValue(Prop::backgroundColor, Vector4(255, 255, 0, 255));
        yellow.SetActiveState(SDLCore::UI::UIState::HOVER);
        yellow.SetValue(Prop::backgroundColor, Vector4(155, 155, 0, 255));
        yellow.SetValue(Prop::width, 230.0f);

        UI::UIStyle px200;
        px200.SetValue(Prop::width, 150.0f).SetValue(Prop::height, 150.0f);

        UI::BeginFrame(UI::UIKey("root"), styleRoot);
        {
            UI::BeginFrame(UI::UIKey("1"), px200, blue);
            UI::EndFrame();

            UI::BeginFrame(UI::UIKey("2"), px200, green);
            UI::EndFrame();

            UI::BeginFrame(UI::UIKey("3"), px200, yellow);
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