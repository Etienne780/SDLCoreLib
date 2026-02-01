#pragma once
#include <functional>

#include "Layer.h"
#include "App.h"

class MainMenuLayer : public Layer {
public:
	MainMenuLayer(App* app);

	void Update() override;
	void Render() override;

private:
	SDLCore::UI::UIContext* m_context = nullptr;

	void RenderBaseBTN(const std::string& text, 
		const SDLCore::UI::UIStyle& btnStyle, 
		const std::function<void()>& action);
};
