#pragma once
#include "Layer.h"
#include "App.h"

class GameLayer : public Layer {
public:
	GameLayer(App* app);

	void Update() override;
	void Render() override;

private:
	SDLCore::UI::UIContext* m_context = nullptr;
};
