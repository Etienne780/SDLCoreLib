#pragma once
#include <SDL3_mixer/SDL_mixer.h>
#include "Types/Types.h"

namespace SDLCore {
    /*
    |Audio - Type	                    | Duration  | Recommendation	        | Reason
    |------------------------------------------------------------------------------------------------------------
    |SFX (Sound Effects)                | < 2 s     | PREDECODED	            | Lowest latency, minimal RAM usage
    |Medium-length Effects	            | >2 s    | NOT_PREDECODED	        | Consider RAM vs. number of simultaneous instances
    */

    class SoundManager;

    enum class SoundType {
        AUTO = 0,       /**< Selects automaticly wich type to use depending on its length. (< 2s = Predecoded; 2-10s = Not predecoded; >10s = Stream)*/
        PREDECODED,     /**< Lowest latency, highest RAM usage */
        NOT_PREDECODED, /**< Moderate latency, reduced RAM usage */
    };

    class SoundClip {
        friend class SoundManager;
    public:
        SoundClip() = default;

        /**
        * @brief Constructs a sound clip from a file.
        * @param path Path to the audio file.
        * @param type Sound type used for loading (default AUTO).
        * @return true on success, false on failure. On failure, the object will
        *         contain SDLCORE_INVALID_ID. Call SDLCore::GetError() for details.
        */
        SoundClip(const SystemFilePath& path, SoundType type = SoundType::AUTO);
        SoundClip(const SoundClip& other);
        SoundClip(SoundClip&& other) noexcept;
        ~SoundClip();

        SoundClip& operator=(const SoundClip& other);
        SoundClip& operator=(SoundClip&& other) noexcept;

        /*
        * @brief Gets the id of this sound object
        * @return SoundClipID of this object
        */
        SoundClipID GetID() const;
        SoundType GetSoundType() const;

        float GetVolume() const;
        float GetDurationMS() const;
        float GetDurationSec() const;
        Sint64 GetNumberOfFrames() const;
        int GetFrequency() const;

        /*
        * @brief 0 is muted and 1 is the default
        */
        SoundClip* SetVolume(float volume);

    private:
        static inline constexpr float autoPredecodeThresholdMS = 2000.0f;   // upper bound for PREDECODED
        static inline IDManager idManager{ IDOrder::ASCENDING };

        SoundClipID m_id{ SDLCORE_INVALID_ID };
        SoundType m_type = SoundType::AUTO;
        SystemFilePath m_path;
        float m_volume = 0.5f;
        float m_durationMS = 0.0f;

        // Total number of audio frames in the clip. can be MIX_DURATION_UNKNOWN, MIX_DURATION_INFINITE
        // A frame represents a single sample per channel (e.g., for stereo, one frame includes left and right samples).
        Sint64 m_frameCount = 0;

        // Audio sample rate in Hz (frames per second).
        // Determines how many frames are played per second.
        int m_frequency = 0;
        float m_pitch = 0;
        float m_pan = 0;

        /*
        * @return true on success. Call SDLCore::GetError() for more information
        */
        bool LoadSound(const SystemFilePath& path, SoundType type);

        /*
        * @brief loads audio staticly 
        * @return true on success. Call SDLCore::GetError() for more information
        */
        bool CreateStaticAudio(const char* path, bool predecode);
    };

}

template<>
static inline std::string FormatUtils::toString<SDLCore::SoundType>(SDLCore::SoundType type) {
    switch (type)
    {
    case SDLCore::SoundType::PREDECODED:        return "Predecoded";
    case SDLCore::SoundType::NOT_PREDECODED:    return "Not Predecoded";
    default:                                    return "UNKNOWN";
    }
}