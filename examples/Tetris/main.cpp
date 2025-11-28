#include <CoreLib/Log.h>
#include <SDLCoreLib/SDLCore.h>
#include <SDL3/SDL_main.h>

#include "Tetris.h"

int main(int argc, char* argv[]) {
	Tetris* tetris = new Tetris();
	SDLCore::SDLResult result = tetris->Start();

	std::string msg = SDLCore::GetError(result);
	if(result == 0)
		Log::Info(msg);
	else 
		Log::Error(msg);

	delete tetris;
	return 0;
}