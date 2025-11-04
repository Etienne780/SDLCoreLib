#include "Lunara.h"


Lunara::Lunara() 
	: Application("Lunara", SDLCore::Version(1, 0)) {
}

static SDLCore::WindowID winLunaraID;
static SDLCore::WindowID winZweiID;
void Lunara::OnStart() {
	auto* winLunara = CreateWindow("Lunara", 200, 200);
	winLunaraID = winLunara->GetID();
	winLunara->SetOnClose([]() { winLunaraID.value = SDLCORE_INVALID_ID; });

	auto* winZwei = CreateWindow("zwei", 200, 200);
	winZweiID = winZwei->GetID();
	winZwei->SetOnClose([]() { winZweiID.value = SDLCORE_INVALID_ID; });
}

void Lunara::OnUpdate() {
	if (GetWindowCount() <= 0)
		Quit();

	{
		namespace RE = SDLCore::Renderer;

		if (winLunaraID != SDLCORE_INVALID_ID) {
			RE::SetWindowRenderer(winLunaraID);
			RE::SetColor(255, 0, 0);
			RE::Clear();

			RE::SetColor(0, 255, 0);
			RE::FillRect(100, 100, 50, 50);

			RE::Present();
		}

		if (winZweiID != SDLCORE_INVALID_ID) {
			RE::SetWindowRenderer(winZweiID);
			RE::SetColor(0, 0, 255);
			RE::Clear();

			RE::SetColor(0, 255, 0);
			RE::FillRect(100, 100, 50, 50);

			RE::Present();
		}
	}
}

void Lunara::OnQuit() {
	
}