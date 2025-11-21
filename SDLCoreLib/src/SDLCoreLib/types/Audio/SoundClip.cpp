#include "SDLCoreError.h"
#include "Types/Audio/SoundManager.h"
#include "Types/Audio/SoundClip.h"

namespace SDLCore {

	SoundClip::SoundClip(const SystemFilePath& filePath, SoundType type) {
		if (!File::Exists(filePath)) {
			Log::Error("SDLCore::SoundClip: Could not create Sound clip! Path '{}' dosent exist!", filePath);
			return;
		}

		if (!LoadSound(filePath, type)) {
			Log::Error(GetError());
            return;
		}
	}

	SoundClip::~SoundClip() {
	
	}

    bool SoundClip::LoadSound(const SystemFilePath& path, SoundType type) {
        MIX_Mixer* mixer = SoundManager::GetMixer();
        if (!mixer) {
            AddError("\nSDLCore::SoundClip::LoadSound: Could not load sound! mixer is null");
            return false;
        }

        MIX_Audio* audio = MIX_LoadAudio(mixer, path.string().c_str(), true);
        if (!audio) {
            SetError("SDLCore::SoundClip::LoadSound: Failed to load audio: " + std::string(SDL_GetError()));
            return false;
        }

        Sint64 frames = MIX_GetAudioDuration(audio);

        SDL_AudioSpec spec;
        MIX_GetAudioFormat(audio, &spec);

        // Convert frames to milliseconds
        m_durationMS = (double)frames * 1000.0 / spec.freq;

        if (frames == MIX_DURATION_UNKNOWN || frames == MIX_DURATION_INFINITE) {
            // default audio type if frames is unkown
            if (type == SoundType::AUTO) {
                type = SoundType::STREAM;
            }
        }
        else {
            if (type == SoundType::AUTO) {
                if (m_durationMS < autoPredecodeThresholdMS) {
                    type = SoundType::PREDECODED;
                }
                else if (m_durationMS < autoStreamThresholdMS) {
                    type = SoundType::NOT_PREDECODED;
                }
                else {
                    type = SoundType::STREAM;
                }
            }
        }

        MIX_DestroyAudio(audio);
        
        if (type == SoundType::STREAM) {
            SDL_PropertiesID props = SDL_CreateProperties();
            SDL_SetBooleanProperty(props, MIX_AUDIOPROP_STREAM, true);
            MIX_PROP_AUDIO_LOAD_CLOSEIO_BOOLEAN
            MIX_Audio* audio = MIX_LoadAudioWithProperties(mixer, path, props);
            SDL_AudioStream* stream;
        }
        else {
            bool predecode = (type == SoundType::PREDECODED);
            audio = MIX_LoadAudio(mixer, path.string().c_str(), predecode);
        }

        if (!audio) {
            SetError("Failed to reload audio: " + std::string(SDL_GetError()));
            return false;
        }

        m_type = type;
        m_audio = audio;
        return true;
	}

}