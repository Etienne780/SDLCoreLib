#include <CoreLib/Log.h>
#include <SDLCoreLib/SDLCore.h>
#include <SDL3/SDL_main.h>

#include "Demo.h"

int main(int argc, char* argv[]) {
	Demo* demo = new Demo();
	SDLCore::SDLResult result = demo->Start();

	std::string msg = SDLCore::GetError(result);
	if(result == 0)
		Log::Info(msg);
	else 
		Log::Error(msg);

	delete demo;
	return 0;
}