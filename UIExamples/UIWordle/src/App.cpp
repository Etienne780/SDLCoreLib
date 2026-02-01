#include "App.h"

#include "MainMenuLayer.h"
#include "GameLayer.h"

App::App()
    : Application("Wordle", SDLCore::Version(1, 0)) {
}

void App::OnStart() {
    SDLCore::Texture tex("J:/images/image.png");
    auto* win = CreateWindow(&m_winID, "Wordle", 900, 900);
    win->SetIcon(tex);
    win->SetWindowMinSize(900, 900);

    InitSpaces();
    UpdateFontSizes();
    UpdateColorTheme();
    InitBaseStyles();
    PushLayer<MainMenuLayer>();
}

void App::OnUpdate() {

    // Quit if no windows remain
    if (GetWindowCount() <= 0)
        Quit();

    if (!m_winID.IsInvalid() && m_currentLayer) {
        using namespace SDLCore;
        Input::SetWindow(m_winID);

        // change layer 
        if (m_pendingLayerChange) {
            m_pendingLayerChange();
            m_pendingLayerChange = nullptr;
        }

        m_currentLayer->Update();

        namespace RE = SDLCore::Render;
        RE::SetWindowRenderer(m_winID);
        RE::SetBlendMode(SDLCore::Render::BlendMode::BLEND);
        RE::SetColor(0);
        RE::Clear();

        m_currentLayer->Render();

        RE::Present();

        if (Input::KeyJustPressed(KeyCode::ESCAPE))
            DeleteWindow(m_winID);
    }
}

void App::OnQuit() {
}

bool App::IsDarkMode() const {
    return m_isDarkMode;
}

SDLCore::WindowID App::GetWinID() const {
    return m_winID;
}

Layer* App::GetCurrentLayer() const {
    return m_currentLayer.get();
}

float App::GetFontSizeScaler() const {
    return m_fontSizeScaler;
}

void App::SetDarkMode(bool value) {
    m_isDarkMode = value;
    UpdateColorTheme();
}

void App::SetFontSizeScaler(float scaler) {
    m_fontSizeScaler = scaler;
    UpdateFontSizes();
}

void App::InitSpaces() {
    using namespace SDLCore::UI;

    float base = 4;
    m_spaceXXS = UIRegistry::RegisterNumber(base);
    m_spaceXS = UIRegistry::RegisterNumber(base * 2);
    m_spaceS = UIRegistry::RegisterNumber(base  * 3);
    m_spaceM = UIRegistry::RegisterNumber(base  * 4);
    m_spaceL = UIRegistry::RegisterNumber(base  * 5);
}

void App::UpdateFontSizes() {
    using namespace SDLCore::UI;

    float& scaler = m_fontSizeScaler;

    m_fontSizeXXS = UIRegistry::RegisterNumber(10 * scaler);
    m_fontSizeXS = UIRegistry::RegisterNumber(12 * scaler);
    m_fontSizeS = UIRegistry::RegisterNumber(14 * scaler);
    m_fontSizeM = UIRegistry::RegisterNumber(16 * scaler);
    m_fontSizeL = UIRegistry::RegisterNumber(20 * scaler);
    m_fontSizeXL = UIRegistry::RegisterNumber(24 * scaler);
}

void App::UpdateColorTheme() {
    using namespace SDLCore::UI;

    if (m_isDarkMode) {
        // -----------------------
        // Background (Dark)
        // -----------------------
        m_backgroundPrimaryColor = UIRegistry::RegisterColor(Vector4(30, 30, 30, 255));   // dark gray
        m_backgroundPrimaryColorInvert = UIRegistry::RegisterColor(Vector4(245, 245, 245, 255)); // white-ish
        m_backgroundSecondaryColor = UIRegistry::RegisterColor(Vector4(86, 150, 80, 255)); // green (success)
        m_backgroundTertiaryColor = UIRegistry::RegisterColor(Vector4(181, 160, 68, 255)); // yellow (warning)

        // -----------------------
        // Text (Dark)
        // -----------------------
        m_textPrimaryColor = UIRegistry::RegisterColor(Vector4(245, 245, 245, 255)); // white
        m_textPrimaryColorInvert = UIRegistry::RegisterColor(Vector4(30, 30, 30, 255));   // dark
        m_textSecondaryColor = UIRegistry::RegisterColor(Vector4(200, 200, 200, 255)); // light gray
        m_textDisabledColor = UIRegistry::RegisterColor(Vector4(120, 124, 126, 255)); // gray

        // -----------------------
        // Feedback (Dark)
        // -----------------------
        m_colorSuccess = UIRegistry::RegisterColor(Vector4(86, 150, 80, 255));  // green
        m_colorWarning = UIRegistry::RegisterColor(Vector4(181, 160, 68, 255)); // yellow
        m_colorError = UIRegistry::RegisterColor(Vector4(220, 53, 69, 255)); // red

        // -----------------------
        // Interactive UI Elements (Dark)
        // -----------------------
        m_buttonPrimary.normal = UIRegistry::RegisterColor(Vector4(86, 150, 80, 255));
        m_buttonPrimary.hover = UIRegistry::RegisterColor(Vector4(76, 140, 70, 255));
        m_buttonPrimary.active = UIRegistry::RegisterColor(Vector4(66, 130, 60, 255));
        m_buttonPrimary.disabled = UIRegistry::RegisterColor(Vector4(80, 80, 80, 255));

        m_buttonSecondary.normal = UIRegistry::RegisterColor(Vector4(181, 160, 68, 255));
        m_buttonSecondary.hover = UIRegistry::RegisterColor(Vector4(171, 150, 58, 255));
        m_buttonSecondary.active = UIRegistry::RegisterColor(Vector4(161, 140, 48, 255));
        m_buttonSecondary.disabled = UIRegistry::RegisterColor(Vector4(80, 80, 80, 255));

        m_panelBackground.normal = UIRegistry::RegisterColor(Vector4(50, 50, 50, 255));
        m_panelBackground.hover = UIRegistry::RegisterColor(Vector4(60, 60, 60, 255));
        m_panelBackground.active = UIRegistry::RegisterColor(Vector4(70, 70, 70, 255));
        m_panelBackground.disabled = UIRegistry::RegisterColor(Vector4(80, 80, 80, 255));

        return; // Dark Mode fertig, rest überspringen
    }

    // -----------------------
    // Background (Light)
    // -----------------------
    m_backgroundPrimaryColor = UIRegistry::RegisterColor(Vector4(255, 255, 255, 255));
    m_backgroundPrimaryColorInvert = UIRegistry::RegisterColor(Vector4(0, 0, 0, 255));
    m_backgroundSecondaryColor = UIRegistry::RegisterColor(Vector4(106, 170, 100, 255)); // green
    m_backgroundTertiaryColor = UIRegistry::RegisterColor(Vector4(201, 180, 88, 255));  // yellow

    // -----------------------
    // Text (Light)
    // -----------------------
    m_textPrimaryColor = UIRegistry::RegisterColor(Vector4(0, 0, 0, 255));
    m_textPrimaryColorInvert = UIRegistry::RegisterColor(Vector4(255, 255, 255, 255));
    m_textSecondaryColor = UIRegistry::RegisterColor(Vector4(60, 60, 60, 255));
    m_textDisabledColor = UIRegistry::RegisterColor(Vector4(120, 124, 126, 255));

    // -----------------------
    // Feedback (Light)
    // -----------------------
    m_colorSuccess = UIRegistry::RegisterColor(Vector4(106, 170, 100, 255));
    m_colorWarning = UIRegistry::RegisterColor(Vector4(201, 180, 88, 255));
    m_colorError = UIRegistry::RegisterColor(Vector4(220, 53, 69, 255));

    // -----------------------
    // Interactive UI Elements (Light)
    // -----------------------
    m_buttonPrimary.normal = UIRegistry::RegisterColor(Vector4(106, 170, 100, 255));
    m_buttonPrimary.hover = UIRegistry::RegisterColor(Vector4(96, 160, 90, 255));
    m_buttonPrimary.active = UIRegistry::RegisterColor(Vector4(86, 150, 80, 255));
    m_buttonPrimary.disabled = UIRegistry::RegisterColor(Vector4(180, 180, 180, 255));

    m_buttonSecondary.normal = UIRegistry::RegisterColor(Vector4(201, 180, 88, 255));
    m_buttonSecondary.hover = UIRegistry::RegisterColor(Vector4(191, 170, 78, 255));
    m_buttonSecondary.active = UIRegistry::RegisterColor(Vector4(181, 160, 68, 255));
    m_buttonSecondary.disabled = UIRegistry::RegisterColor(Vector4(180, 180, 180, 255));

    m_panelBackground.normal = UIRegistry::RegisterColor(Vector4(245, 245, 245, 255));
    m_panelBackground.hover = UIRegistry::RegisterColor(Vector4(235, 235, 235, 255));
    m_panelBackground.active = UIRegistry::RegisterColor(Vector4(225, 225, 225, 255));
    m_panelBackground.disabled = UIRegistry::RegisterColor(Vector4(200, 200, 200, 255));
}

void App::InitBaseStyles() {
    using namespace SDLCore;
    namespace Prop = SDLCore::UI::Properties;
    
    m_styleBaseBTN.SetValue(Prop::align, UI::UIAlignment::CENTER, UI::UIAlignment::CENTER)
        .SetValue(Prop::backgroundColor, m_buttonPrimary.normal)
        .SetValue(Prop::duration, 0.075f)
        .SetValue(Prop::sizeUnit, UI::UISizeUnit::PERCENTAGE, UI::UISizeUnit::PERCENTAGE)
        .SetValue(Prop::size, 100.0f, 100.0f)
        .SetValue(Prop::hideOverflow, true, true);

    m_styleBaseBTN.SetActiveState(UI::UIState::HOVER)
        .SetValue(Prop::backgroundColor, m_buttonPrimary.hover);

    m_styleBaseBTN.SetActiveState(UI::UIState::PRESSED)
        .SetValue(Prop::backgroundColor, m_buttonPrimary.active);
}