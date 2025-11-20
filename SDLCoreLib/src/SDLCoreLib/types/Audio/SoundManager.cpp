#include <memory>
#include <CoreLib/Log.h>

#include "Application.h"
#include "SDLCoreError.h"
#include "Types/Audio/SoundClip.h"
#include "Types/Audio/SoundManager.h"

namespace SDLCore {
	
	static SoundManager* s_soundManager = nullptr;

	SoundManager::~SoundManager() {
		Cleanup();
	}

	#pragma region Static

	bool SoundManager::Init(SDL_AudioDeviceID audio) {
		// Quit current SoundManager if exist
		Quit();

		s_soundManager = new SoundManager();
		s_soundManager->m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
		if (!s_soundManager->m_mixer) {
			SetError(Log::GetFormattedString(
				"SDLCore::SoundManager::Init: Faild to create mixer! {}", 
				SDL_GetError()));
			return false;
		}

		if (!s_soundManager->CreateDevices())
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

	bool SoundManager::InstanceExist() {
		if (!s_soundManager) {
			SetError("SDLCore::SoundManager::InstanceExist: No valid instance of SoundManager exists!");
			return false;
		}

		return true;
	}

	MIX_Mixer* SoundManager::GetMixer() {
		if (!InstanceExist())
			return nullptr;
		return s_soundManager->m_mixer;
	}

	bool SoundManager::SetAudioDevice(AudioPlaybackDeviceID deviceID) {
		if (!InstanceExist())
			return false;

		s_soundManager->Cleanup();
		bool result = true;

		SDL_AudioDeviceID targetSDLID = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;
		auto& devices = s_soundManager->m_devices;
		auto& mixer = s_soundManager->m_mixer;
		if (deviceID != 0) {
			auto it = std::find_if(devices.begin(), devices.end(),
				[deviceID](const AudioPlaybackDevice& dev) { return dev.GetID() == deviceID; });

			if (it != devices.end()) {
				targetSDLID = it->GetSDLID();
			}
			else {
				SetError(Log::GetFormattedString(
					"SDLCore::SoundManager::SetAudioDevice: Device with id '{}' not found! Using default device",
					deviceID));
				result = false;
			}
		}

		mixer = MIX_CreateMixerDevice(targetSDLID, nullptr);

		if (!mixer) {
			const std::string err = Log::GetFormattedString(
				"SDLCore::SoundManager::SetAudioDevice: {}", SDL_GetError());

			if (result)
				SetError(err);
			else
				AddError(err);

			result = false;
		}

		return result;
	}

	#pragma endregion

	#pragma region Member

	void SoundManager::Cleanup() {
		MIX_DestroyMixer(m_mixer);
	}

	bool SoundManager::CreateDevices() {
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

	#pragma endregion

}