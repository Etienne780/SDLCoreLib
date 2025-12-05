#pragma once
#include <CoreLib/Math/Vector2.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "Types/Types.h"

namespace SDLCore {
    /*
    * @Note
    * I don like the structure of sub sounds. It should exist a soundID and a 
    * sound handle id. The sound id is a map fom id -> MIX_Audio* and a soundID can be
    * created with a SoundID. 
    * The SoundManager could than play a SoundClip object that contains a SoundHandelID 
    * and a SoundID. The SoundID is for what to play and the SoundHandelID is for 
    * how, like volume, position, ...
    */

    /*
    |Audio - Type	                    | Duration  | Recommendation	        | Reason
    |------------------------------------------------------------------------------------------------------------
    |SFX (Sound Effects)                | < 2 s     | PREDECODED	            | Lowest latency, minimal RAM usage
    |Medium-length Effects	            | > 2 s     | NOT_PREDECODED	        | Consider RAM vs. number of simultaneous instances
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

        SoundClip CreateSubSound() const;

        bool IsSubSound() const;

        /**
        * @brief Returns the primary identifier of this sound clip.
        * @return SoundClipID associated with this clip.
        */
        SoundClipID GetID() const;

        /**
        * @brief Returns the secondary identifier of this sound clip.
        * @return Sub-identifier of this clip.
        */
        SoundClipID GetSubID() const;

        /**
        * @brief Returns the type classification of the sound clip.
        * @return SoundType enum value representing the clip type.
        */
        SoundType GetSoundType() const;

        /**
        * @brief Returns the current gain factor of the sound clip.
        * @return Volume value, where 0.0 is muted and 1.0 is the default level.
        */
        float GetVolume() const;

        /**
        * @brief Returns the configured loop count for playback.
        * @return Number of loops; -1 indicates infinite looping.
        */
        int GetNumberOfLoops() const;

        /**
        * @brief Returns the clip duration in milliseconds.
        * @return Duration in ms.
        */
        float GetDurationMS() const;

        /**
        * @brief Returns the clip duration in seconds.
        * @return Duration in seconds.
        */
        float GetDurationSec() const;

        /**
        * @brief Returns the total number of audio frames in this clip.
        * @return Frame count.
        */
        Sint64 GetNumberOfFrames() const;

        /**
        * @brief Returns the sampling frequency of the clip.
        * @return Frequency in Hz.
        */
        int GetFrequency() const;

        /**
        * @brief Returns the spatial position assigned to this clip.
        * @return Position vector used for 2D positional audio.
        */
        Vector2 GetPosition() const;
        std::string GetName() const;
        SystemFilePath GetPath() const;

        /**
        * @brief Sets the linear gain factor of the clip.
        * @param volume Gain value; 0.0 mutes, 1.0 is the default level.
        * @return Pointer to this clip instance.
        */
        SoundClip* SetVolume(float volume);

        /**
        * @brief Sets how often the clip should loop during playback.
        * @param value Loop count; 0 disables looping, 1 repeats once, -1 enables infinite looping.
        * @return Pointer to this clip instance.
        *
        * @note Loop configuration is applied through SDL mixer properties.
        *       When used with track tags, behavior may vary depending on backend support.
        */
        SoundClip* SetNumberOfLoops(int value);

        /**
        * @brief Sets the spatial position of the clip in 2D space.
        * @param x X-coordinate.
        * @param y Y-coordinate.
        * @return Pointer to this clip instance.
        */
        SoundClip* SetPosition(float x, float y);

        /**
        * @brief Sets the spatial position of the clip in 2D space.
        * @param pos Position vector.
        * @return Pointer to this clip instance.
        */
        SoundClip* SetPosition(const Vector2& pos);

        /**
        * @brief Sets the 2D position and volume of the sound relative to a listener.
        * @param soundPos Absolute 2D position of the sound.
        * @param listenerPos Absolute 2D position of the listener.
        * @param maxDistance Maximum distance for attenuation.
        * @param maxVolume Maximum volume (0.0 = silent, 1.0 = full volume).
        * @return Pointer to this SoundClip.
        */
        SoundClip* Set2D(const Vector2& soundPos, const Vector2& listenerPos, float maxDistance = 800.0f, float maxVolume = 1.0f);

    private:
        static inline constexpr float autoPredecodeThresholdMS = 2000.0f;   // upper bound for PREDECODED
        static inline IDManager idManager{ IDOrder::ASCENDING };

        SoundClipID m_id{ SDLCORE_INVALID_ID };
        SoundClipID m_subID{ SDLCORE_INVALID_ID };
        SoundType m_type = SoundType::AUTO;
        SystemFilePath m_path;
        int m_numberOfLoops = 0;
        float m_volume = 0.5f;
        float m_durationMS = 0.0f;

        // Total number of audio frames in the clip. can be MIX_DURATION_UNKNOWN, MIX_DURATION_INFINITE
        // A frame represents a single sample per channel (e.g., for stereo, one frame includes left and right samples).
        Sint64 m_frameCount = 0;

        // Audio sample rate in Hz (frames per second).
        // Determines how many frames are played per second.
        int m_frequency = 0;
        Vector2 m_position;

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
static inline std::string FormatUtils::toString<SDLCore::SoundClip>(SDLCore::SoundClip clip) {
    const bool hasName = !clip.GetName().empty();
    const bool isSub = clip.IsSubSound();

    if (isSub) {
        if (hasName) {
            return FormatUtils::formatString(
                "SoundClip[name='{}', id={}, sub={}]",
                clip.GetName(),
                clip.GetID(),
                clip.GetSubID()
            );
        }

        return FormatUtils::formatString(
            "SoundClip[id={}, sub={}]",
            clip.GetID(),
            clip.GetSubID()
        );
    }

    if (hasName) {
        return FormatUtils::formatString(
            "SoundClip[name='{}', id={}]",
            clip.GetName(),
            clip.GetID()
        );
    }

    return FormatUtils::formatString(
        "SoundClip[id={}]",
        clip.GetID()
    );
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