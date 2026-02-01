#include <SDLCoreLib/SDLCore.h>
#include <SDLCoreLib/SDLCoreUI.h>

#include "MainMenuLayer.h"
#include "GameLayer.h"

static SDLCore::UI::UIStyle styleRoot{ "root" };
static SDLCore::UI::UIStyle styleHeader{ "header" };
static SDLCore::UI::UIStyle styleBody{ "body" };
static SDLCore::UI::UIStyle styleFooter{ "footer" };

static SDLCore::UI::UIStyle styleBodyConainer{ "bodyContainer" };
static SDLCore::UI::UIStyle styleBodyConainerBTN{ "bodyContainerBTN" };

static SDLCore::UI::UIStyle styleText{ "text" };
static SDLCore::UI::UIStyle styleNanoText{ "nanoText" };
static SDLCore::UI::UIStyle styleMicroText{ "microText" };
static SDLCore::UI::UIStyle styleSmallText{ "smallText" };
static SDLCore::UI::UIStyle styleTitleText{ "titleText" };

MainMenuLayer::MainMenuLayer(App* app) 
	: Layer(app) {
    using namespace SDLCore;
    namespace Prop = SDLCore::UI::Properties;

    m_context = SDLCore::UI::CreateContext();

    styleRoot.SetValue(Prop::backgroundColor, m_app->m_backgroundPrimaryColor)
        .SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
        .SetValue(Prop::sizeUnit, UI::UISizeUnit::PERCENTAGE, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::size, 100.0f, 100.0f);

    styleHeader
        .SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
        .SetValue(Prop::sizeUnit, UI::UISizeUnit::PERCENTAGE, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::size, 100.0f, 25.0f);

    styleBody.SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::START)
        .SetValue(Prop::sizeUnit, UI::UISizeUnit::PERCENTAGE, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::size, 100.0f, 65.0f)
        .SetValue(Prop::padding, m_app->m_spaceL);

    styleFooter.SetValue(Prop::layoutDirection, UI::UILayoutDir::ROW)
        .SetValue(Prop::align, UI::UIAlignment::END, UI::UIAlignment::END)
        .SetValue(Prop::sizeUnit, UI::UISizeUnit::PERCENTAGE, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::size, 100.0f, 10.0f)
        .SetValue(Prop::padding, m_app->m_spaceL);

    styleBodyConainer.SetValue(Prop::layoutDirection, UI::UILayoutDir::COLUMN)
        .SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
        .SetValue(Prop::sizeUnit, UI::UISizeUnit::PERCENTAGE, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::padding, m_app->m_spaceXS)
        .SetValue(Prop::size, 50.0f, 50.0f);

    styleBodyConainerBTN.Merge(m_app->m_styleBaseBTN)
        .SetValue(Prop::height, 100.0f/3);

    styleText.SetValue(Prop::textColor, m_app->m_textPrimaryColor)
        .SetValue(Prop::textSize, m_app->m_fontSizeM);
    styleNanoText.Merge(styleText)
        .SetValue(Prop::textSize, m_app->m_fontSizeXXS);
    styleMicroText.Merge(styleText)
        .SetValue(Prop::textSize, m_app->m_fontSizeXS);
    styleSmallText.Merge(styleText)
        .SetValue(Prop::textSize, m_app->m_fontSizeS);
    styleTitleText.Merge(styleText)
        .SetValue(Prop::textSize, m_app->m_fontSizeXL);
}

void MainMenuLayer::Update() {
    
}

void MainMenuLayer::Render() {
    using namespace SDLCore;
    namespace Prop = SDLCore::UI::Properties;

    UI::SetContextWindow(m_context, m_app->GetWinID());
    UI::BindContext(m_context);

    UI::BeginFrame(UI::UIKey("root"), styleRoot);
    {
        UI::BeginFrame(UI::UIKey("header"), styleHeader);
        {
            UI::Text(UI::UIKey("title"), "Wordle", styleTitleText);
        }
        UI::EndFrame();

        UI::BeginFrame(UI::UIKey("body"), styleBody);
        {
            UI::BeginFrame(UI::UIKey("bodyContainer"), styleBodyConainer);
            {
                RenderBaseBTN("Play", styleBodyConainerBTN, [&]() { m_app->RequestPushLayer<GameLayer>(); });
                RenderBaseBTN("Settings", styleBodyConainerBTN, nullptr);
                RenderBaseBTN("Quit", styleBodyConainerBTN, [&]() { m_app->Quit(); });
            }
            UI::EndFrame();
        }
        UI::EndFrame();

        UI::BeginFrame(UI::UIKey("footer"), styleFooter);
        {
            UI::Text(UI::UIKey("version"), "Version: " + m_app->GetVersion().ToString(), styleNanoText);
        }
        UI::EndFrame();
    }
    UI::EndFrame();
}

void MainMenuLayer::RenderBaseBTN(const std::string& text, 
    const SDLCore::UI::UIStyle& btnStyle, 
    const std::function<void()>& action) 
{
    using namespace SDLCore;
    namespace Prop = SDLCore::UI::Properties;
    
    UI::BeginFrame(UI::UIKey("btn_" + text), btnStyle);
    {
        UI::Text(UI::UIKey("Text"), text, styleText)->SetOverride(Prop::hitTestTransparent, true);
    }
    UI::UIEvent event = UI::EndFrame();

    if (!action)
        return;

    if (event.IsPressed())
        action();
}