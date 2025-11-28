#include <CoreLib/Log.h>
#include <SDLCoreLib/SDLCore.h>
#include <SDL3/SDL_main.h>

#include "Pong.h"

int main(int argc, char* argv[]) {
	Pong* pong = new Pong();
	SDLCore::SDLResult result = pong->Start();

	std::string msg = SDLCore::GetError(result);
	if(result == 0)
		Log::Info(msg);
	else 
		Log::Error(msg);

	delete pong;
	return 0;
}