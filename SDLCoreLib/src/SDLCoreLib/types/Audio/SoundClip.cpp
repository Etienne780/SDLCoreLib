#include "SDLCoreError.h"
#include "Application.h"
#include "Types/Audio/SoundManager.h"
#include "Types/Audio/SoundClip.h"

namespace SDLCore {

	SoundClip::SoundClip(const SystemFilePath& filePath, SoundType type) 
        : m_path(filePath) {
        LoadSound(filePath, type);
	}

	SoundClip::~SoundClip() {
        SoundManager::DeletedSound(m_id);
        idManager.FreeUniqueIdentifier(m_id.value);
	}

    SoundClipID SoundClip::GetID() const {
        return m_id;
    }

    SoundType SoundClip::GetSoundType() const {
        return m_type;
    }
    
    float SoundClip::GetVolume() const {
        return m_volume;
    }

    float SoundClip::GetDurationMS() const {
        return m_durationMS;
    }

    float SoundClip::GetDurationSec() const {
        return m_durationMS / 1000;
    }

    Sint64 SoundClip::GetNumberOfFrames() const {
        return m_frameCount;
    }
    
    int SoundClip::GetFrequency() const {
        return m_frequency;
    }

    SoundClip* SoundClip::SetVolume(float volume) {
        m_volume = volume;
        return this;
    }

    bool SoundClip::LoadSound(const SystemFilePath& path, SoundType type) {
        if (!File::Exists(path)) {
            SetErrorF("SDLCore::SoundClip::LoadSound: Failed to load audio, file '{}' dose not exist!", 
                path);
            return false;
        }

        std::string strPath = path.string();
        MIX_Audio* tempAudio = MIX_LoadAudio(nullptr, strPath.c_str(), true);
        if (!tempAudio) {
            SetError("SDLCore::SoundClip::LoadSound: Failed to load audio!\n" + std::string(SDL_GetError()));
            return false;
        }

        m_frameCount = MIX_GetAudioDuration(tempAudio);

        SDL_AudioSpec spec;
        MIX_GetAudioFormat(tempAudio, &spec);
        m_frequency = spec.freq;

        // Convert frames to milliseconds
        if (m_frameCount > 0 && m_frequency > 0)
            m_durationMS = static_cast<float>(m_frameCount) * 1000.0f / static_cast<float>(m_frequency);
        else
            m_durationMS = 0.0f;

        MIX_DestroyAudio(tempAudio);
        tempAudio = nullptr;

        if (type == SoundType::AUTO) {
            if (m_frameCount == MIX_DURATION_UNKNOWN || m_frameCount == MIX_DURATION_INFINITE) {
                type = SoundType::NOT_PREDECODED;
            }
            else {
                type = (m_durationMS < autoPredecodeThresholdMS) ? 
                    SoundType::PREDECODED : SoundType::NOT_PREDECODED;
            }
        }
        
        bool predecode = (type == SoundType::PREDECODED);
        if (!CreateStaticAudio(strPath.c_str(), predecode)) {
            AddError("\nSDLCore::SoundClip::LoadSound: Failed to create static audio!");
            return false;
        }

        m_type = type;
        return true;
	}

    bool SoundClip::CreateStaticAudio(const char* path, bool predecode) {
        MIX_Audio* audio = MIX_LoadAudio(nullptr, path, predecode);
        if (!audio) {
            SetErrorF("SDLCore::SoundClip::CreateStaticAudio: Failed to load audio '{}'! \n{}", 
                path, SDL_GetError());
            return false;
        }

        // frees the old id if it exits
        if (m_id != SDLCORE_INVALID_ID) {
            idManager.FreeUniqueIdentifier(m_id.value);
        }
        m_id = SoundClipID(idManager.GetNewUniqueIdentifier());

        // gives owner ship to Sound manager;
        if (!SoundManager::CreateSound(m_id, audio)) {
            AddError("\nSDLCore::SoundClip::CreateStaticAudio: Could not add sound to sound manager!");
            return false;
        }

        return true;
    }

}