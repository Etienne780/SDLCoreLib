#include "Lunara.h"

Lunara::Lunara() 
	: Application("Lunara", SDLCore::Version(1, 0)) {
}

void Lunara::OnStart() {
	CreateWindow("Lunara", 200, 200);
}

void Lunara::OnUpdate() {
	if (GetWindowCount() <= 0)
		Quit();
}

void Lunara::OnQuit() {
	
}