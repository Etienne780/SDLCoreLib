#include <SDLCoreLib/SDLCore.h>
#include <SDLCoreLib/SDLCoreUI.h>

#include "GameLayer.h"

static SDLCore::UI::UIStyle styleRoot{ "root" };

GameLayer::GameLayer(App* app) 
	: Layer(app) {
    using namespace SDLCore;
    namespace Prop = SDLCore::UI::Properties;

    m_context = SDLCore::UI::CreateContext();

    styleRoot.SetValue(Prop::backgroundColor, Vector4(0, 0, 0, 0))
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
        .SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
        .SetValue(Prop::sizeUnit, UI::UISizeUnit::PERCENTAGE, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::size, 100.0f, 100.0f);
}

void GameLayer::Update() {

}

void GameLayer::Render() {
    using namespace SDLCore;
    namespace Prop = SDLCore::UI::Properties;

    UI::SetContextWindow(m_context, m_app->GetWinID());
    UI::BindContext(m_context);

    UI::BeginFrame(UI::UIKey("root"), styleRoot);
    {

    }
    UI::EndFrame();
}