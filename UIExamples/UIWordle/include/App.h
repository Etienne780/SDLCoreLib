#pragma once
#include <SDLCoreLib/SDLCore.h>
#include <SDLCoreLib/SDLCoreUI.h>

#include "Wordle.h"
#include "Layer.h"

struct UIColorSet {
	SDLCore::UI::UIColorID normal;
	SDLCore::UI::UIColorID hover;
	SDLCore::UI::UIColorID active;
	SDLCore::UI::UIColorID disabled;
};

class App : public SDLCore::Application {
public:
	App();

	void OnStart() override;
	void OnUpdate() override;
	void OnQuit() override;

	template<typename T, typename... Args>
	void RequestPushLayer(Args&&... args) {
		m_pendingLayerChange = [this, args...]() {
			PushLayer<T>(args...);
		};
	}

	bool IsDarkMode() const;

	SDLCore::WindowID GetWinID() const;
	Layer* GetCurrentLayer() const;
	float GetFontSizeScaler() const;

	void SetDarkMode(bool value);
	void SetFontSizeScaler(float scaler);

	// -----------------------
	// Colors
	// -----------------------
	SDLCore::UI::UIColorID m_backgroundPrimaryColor;       // main background (white)
	SDLCore::UI::UIColorID m_backgroundPrimaryColorInvert; // inverse (black)
	SDLCore::UI::UIColorID m_backgroundSecondaryColor;     // green (success)
	SDLCore::UI::UIColorID m_backgroundTertiaryColor;      // yellow (warning)

	SDLCore::UI::UIColorID m_textPrimaryColor;        // black
	SDLCore::UI::UIColorID m_textPrimaryColorInvert; // white
	SDLCore::UI::UIColorID m_textSecondaryColor;     // dark gray
	SDLCore::UI::UIColorID m_textDisabledColor;      // gray

	SDLCore::UI::UIColorID m_colorSuccess; // green
	SDLCore::UI::UIColorID m_colorWarning; // yellow
	SDLCore::UI::UIColorID m_colorError;   // red

	// Interactive UI Elements
	UIColorSet m_buttonPrimary;
	UIColorSet m_buttonSecondary;
	UIColorSet m_panelBackground;

	// -----------------------
	// Spaces
	// -----------------------
	SDLCore::UI::UINumberID m_spaceXXS;// 4
	SDLCore::UI::UINumberID m_spaceXS;// m_spaceXXS * 2
	SDLCore::UI::UINumberID m_spaceS;// m_spaceXXS * 3
	SDLCore::UI::UINumberID m_spaceM;// m_spaceXXS * 4
	SDLCore::UI::UINumberID m_spaceL;// m_spaceXXS * 5

	// -----------------------
	// Font sizes
	// -----------------------
	SDLCore::UI::UINumberID m_fontSizeXXS; // 10
	SDLCore::UI::UINumberID m_fontSizeXS;  // 12
	SDLCore::UI::UINumberID m_fontSizeS;   // 14
	SDLCore::UI::UINumberID m_fontSizeM;   // 16
	SDLCore::UI::UINumberID m_fontSizeL;   // 20
	SDLCore::UI::UINumberID m_fontSizeXL;  // 24

	SDLCore::UI::UIStyle m_styleBaseBTN{ "baseBtn" };

private:
	SDLCore::WindowID m_winID;
	std::unique_ptr<Layer> m_currentLayer = nullptr;
	std::function<void()> m_pendingLayerChange;

	bool m_isDarkMode = false;
	float m_fontSizeScaler = 4;

	void InitSpaces();
	void UpdateFontSizes();
	void UpdateColorTheme();

	void InitBaseStyles();

	template<typename T, typename... Args>
	void PushLayer(Args&&... args) {
		static_assert(std::is_base_of_v<Layer, T>, "T must derive from Layer");
		m_currentLayer = std::make_unique<T>(this, std::forward<Args>(args)...);
	}
};