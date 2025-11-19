#include "Types/Audio/AudioPlaybackDevice.h"

namespace SDLCore {

	AudioPlaybackDevice::AudioPlaybackDevice(AudioPlaybackDeviceID id, SDL_AudioDeviceID sdlDeviceID)
		: m_id(id), m_sdlDeviceID(sdlDeviceID), m_name(SDL_GetAudioDeviceName(sdlDeviceID)) {
	}

	AudioPlaybackDeviceID AudioPlaybackDevice::GetID() const {
		return m_id;
	}

	SDL_AudioDeviceID AudioPlaybackDevice::GetSDLID() const {
		return m_sdlDeviceID;
	}

	std::string AudioPlaybackDevice::GetName() const {
		return m_name;
	}

}