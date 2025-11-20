#include "SDLCoreError.h"
#include "Types/Audio/SoundManager.h"
#include "Types/Audio/SoundClip.h"

namespace SDLCore {

	SoundClip::SoundClip(const SystemFilePath& filePath, SoundType type) {
		if (!File::Exists(filePath)) {
			Log::Error("SDLCore::SoundClip: Could not create Sound clip! Path '{}' dosent exist!", filePath);
			return;
		}

		if (!LoadSound(filePath, type, m_audio)) {
			Log::Error(GetError());
		}
	}

	SoundClip::~SoundClip() {
	
	}

	bool SoundClip::LoadSound(const SystemFilePath& path, SoundType type, MIX_Audio*& outAudio) {
		MIX_Mixer* mixer = SoundManager::GetMixer();
		if (!mixer) {
			AddError("\nSDLCore::SoundClip::LoadSound: Could not load sound! mixer is null");
			return false;
		}

		return true;
	}

}