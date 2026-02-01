#pragma once
#include <SDLCoreLib/SDLCore.h>
#include <SDLCoreLib/SDLCoreUI.h>

class App : public SDLCore::Application {
public:
	App();

	void OnStart() override;
	void OnUpdate() override;
	void OnQuit() override;

	void SetupUI();
	void RenderUI(SDLCore::WindowID winID);

private:
	SDLCore::WindowID m_winID;

	SDLCore::UI::UIContext* context = nullptr;
	SDLCore::UI::UIStyle styleRoot{ "root" };
};