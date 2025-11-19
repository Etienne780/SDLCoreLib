#pragma once
#include <string>
#include <SDL3/SDL.h>
#include "Types/Types.h"

namespace SDLCore {

	class AudioPlaybackDevice {
	public:
		AudioPlaybackDevice(AudioPlaybackDeviceID id, SDL_AudioDeviceID sdlDeviceID);

		AudioPlaybackDeviceID GetID() const;
		SDL_AudioDeviceID GetSDLID() const;
		std::string GetName() const;

	private:
		AudioPlaybackDeviceID m_id{ SDLCORE_INVALID_ID };
		SDL_AudioDeviceID m_sdlDeviceID = 0;
		std::string m_name;
	};

}