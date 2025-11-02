#include <CoreLib/Log.h>
#include <SDLCoreLib/Application.h>

#include "Lunara.h"

int main() {
	Lunara lunara;

	SDLCore::SDLResult result = lunara.Start();
	Log::Info(SDLCore::GetError(result));
	
	return 0;
}