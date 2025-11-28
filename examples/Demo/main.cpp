#include <CoreLib/Log.h>
#include <SDLCoreLib/SDLCore.h>
#include <SDL3/SDL_main.h>

#include "App.h"

int main(int argc, char* argv[]) {
	App* app = new App();
	SDLCore::SDLResult result = app->Start();

	std::string msg = SDLCore::GetError(result);
	if(result == 0)
		Log::Info(msg);
	else 
		Log::Error(msg);

	delete app;
	return 0;
}