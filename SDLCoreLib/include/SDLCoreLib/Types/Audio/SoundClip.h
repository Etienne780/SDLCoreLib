#pragma once
#include <SDL3_mixer/SDL_mixer.h>
#include "Types/Types.h"

namespace SDLCore {
    /*
    |Audio - Type	                    | Duration  | Recommendation	        | Reason
    |------------------------------------------------------------------------------------------------------------
    |SFX (Sound Effects)                | < 2 s     | PREDECODED	            | Lowest latency, minimal RAM usage
    |Medium-length Effects	            | 2–10 s    | NOT_PREDECODED	        | Consider RAM vs. number of simultaneous instances
    |Background Music / Long Clips      | >10 s     | STREAM	                | Saves RAM, efficient for large files
    */

    class SoundManager;

    enum class SoundType {
        AUTO = 0,       /**< Selects automaticly wich type to use depending on its length. (< 2s = Predecoded; 2-10s = Not predecoded; >10s = Stream)*/
        PREDECODED,     /**< Lowest latency, highest RAM usage */
        NOT_PREDECODED, /**< Moderate latency, reduced RAM usage */
        STREAM          /**< Highest latency, minimal RAM usage (decoded in chunks) */
    };

    class SoundClip {
        friend class SoundManager;
    public:
        SoundClip(const SystemFilePath& path, SoundType type = SoundType::AUTO);
        ~SoundClip();

    private:
        static inline constexpr float autoPredecodeThresholdMS = 2000.0f;   // upper bound for PREDECODED
        static inline constexpr float autoStreamThresholdMS = 10000.0f;  // upper bound for NOT_PREDECODED

        MIX_Audio* m_audio = nullptr;        // für PREDECODED / NOT_PREDECODED
        SDL_AudioStream* m_stream = nullptr; // für STREAM
        SoundType m_type = SoundType::AUTO;
        float m_volume = 1.0f;
        float m_durationMS = 0.0f;

        /*
        * @return true on success. Call SDLCore::GetError() for more information
        */
        bool LoadSound(const SystemFilePath& path, SoundType type);
    };

}

template<>
static inline std::string FormatUtils::toString<SDLCore::SoundType>(SDLCore::SoundType type) {
    switch (type)
    {
    case SDLCore::SoundType::PREDECODED:        return "Predecoded";
    case SDLCore::SoundType::NOT_PREDECODED:    return "Not Predecoded";
    case SDLCore::SoundType::STREAM:            return "Stream";
    default:                                    return "UNKNOWN";
    }
}