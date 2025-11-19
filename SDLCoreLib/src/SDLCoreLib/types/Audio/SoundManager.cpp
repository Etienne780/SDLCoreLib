#include <memory>
#include <CoreLib/Log.h>

#include "Application.h"
#include "SDLCoreError.h"
#include "Types/Audio/SoundClip.h"
#include "Types/Audio/SoundManager.h"

namespace SDLCore {
	
	static SoundManager* s_soundManager = nullptr;

	SoundManager::~SoundManager() {
		Quit();
	}

	bool SoundManager::Init() {
		// Destroys old mixer and SoundManager instance
		Cleanup();

		s_soundManager = new SoundManager();
		m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
		if (!SetDevices())
			return false;

		return true;
	}

	void SoundManager::Quit() {
		if (s_soundManager) {
			s_soundManager->Cleanup();
			delete s_soundManager;
			s_soundManager = nullptr;
		}
	}

	void SoundManager::Cleanup() {
		MIX_DestroyMixer(m_mixer);
	}

	bool SoundManager::SetAudioDevice(AudioPlaybackDeviceID deviceID) {
		Cleanup();
		bool result = true;

		SDL_AudioDeviceID targetSDLID = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;

		if (deviceID != 0) {
			auto it = std::find_if(m_devices.begin(), m_devices.end(),
				[deviceID](const AudioPlaybackDevice& dev) { return dev.GetID() == deviceID; });

			if (it != m_devices.end()) {
				targetSDLID = it->GetSDLID();
			}
			else {
				SetError(Log::GetFormattedString(
					"SDLCore::SoundManager: Device with id '{}' not found! Using default device",
					deviceID));
				result = false;
			}
		}

		m_mixer = MIX_CreateMixerDevice(targetSDLID, nullptr);

		if (!m_mixer) {
			const std::string err = Log::GetFormattedString(
				"SDLCore::SoundManager: {}", SDL_GetError());

			if (result)
				SetError(err);
			else
				AddError(err);

			result = false;
		}

		return result;
	}

	bool SoundManager::SetDevices() {
		int count = 0;
		SDL_AudioDeviceID* pDevice = SDL_GetAudioPlaybackDevices(&count);
		
		if (!pDevice) {
			SetError(Log::GetFormattedString("SDLCore::SoundManager: {}", SDL_GetError()));
			SDL_free(pDevice);
			return false;
		}
		
		m_deviceIDManager.Reset();
		m_devices.clear();
		m_devices.reserve(count);

		for (int i = 0; i < count; i++) {
			m_devices.emplace_back(
				AudioPlaybackDeviceID(m_deviceIDManager.GetNewUniqueIdentifier()),
				pDevice[i]
			);
		}

		SDL_free(pDevice);
		return true;
	}

}