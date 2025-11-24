#include <CoreLib/Log.h>
#include <SDLCoreLib/SDLCore.h>
#include <SDL3/SDL_main.h>

#include "Lunara.h"

int main() {
	Lunara* lunara = new Lunara();
	SDLCore::SDLResult result = lunara->Start();

	std::string msg = SDLCore::GetError(result);
	if(result == 0)
		Log::Info(msg);
	else 
		Log::Error(msg);

	delete lunara;
	return 0;
}