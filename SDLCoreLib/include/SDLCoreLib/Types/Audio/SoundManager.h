#pragma once
#include <vector>
#include <SDL3_mixer/SDL_mixer.h>

#include "IDManager.h"
#include "SoundClip.h"
#include "AudioPlaybackDevice.h"

namespace SDLCore {
    class Application;

    namespace SoundTags {
        inline constexpr char* DEFAULT = "default";
    }

    inline constexpr bool SOUND_ON_SHOOT = true;

    /*
    * plays sounds mixer and Manages tags. static/but not static like application
    * events wenn sound ends
    * manages tag and master Volumn
    */
    class SoundManager {
    friend class SoundClip;
    friend class Application;
    public:
        ~SoundManager();

        // ============== Static ==============

        static MIX_Mixer* GetMixer();

        /*
        * @brief sets a new audio playback(headphones) device
        * @param deviceID id of the device. 0 is default systme device
        * @return true on success. Call SDLCore::GetError() for more information
        */
        static bool SetAudioDevice(AudioPlaybackDeviceID deviceID);

        /**
        * @brief Removes a SoundClip from the internal storage, stopping playback if necessary.
        *
        * @param clip The SoundClip to remove.
        * @return true on success, false if the removal failed. Call SDLCore::GetError() for details.
        */
        static bool RemoveSound(const SoundClip& clip);

        /**
        * @brief Removes a SoundClip from the internal storage by its ID.
        *
        * @param id The ID of the SoundClip to remove.
        * @return true on success, false if the removal failed. Call SDLCore::GetError() for details.
        */
        static bool RemoveSound(const SoundClipID& id);

        /**
        * @brief Plays a SoundClip.
        *
        * If the clip has a tag, the tag will be used to categorize the sound.
        * If onShoot is true, a temporary one-shot track is created and marked for deletion after playback.
        *
        * @param clip The SoundClip to play.
        * @param onShoot Whether this is a one-shot sound (default: false).
        * @param tag Optional tag for categorization (default: SoundTags::DEFAULT).
        * @return true if the sound was successfully played, false otherwise. Call SDLCore::GetError() for details.
        */
        static bool PlaySound(const SoundClip& clip, bool onShoot = false, const std::string& tag = SoundTags::DEFAULT);

        /**
        * @brief Plays all sounds associated with a specific tag.
        *
        * @param tag The tag identifying which sounds to play.
        * @return true on success, false if playback failed. Call SDLCore::GetError() for more information.
        */
        static bool PlayTag(const std::string& tag);

        /**
        * @brief Pauses a specific SoundClip.
        *
        * @param clip The SoundClip to pause.
        * @return true if the clip was successfully paused, false otherwise.
        */
        static bool PauseSound(const SoundClip& clip);

        /**
        * @brief Pauses all currently playing sounds.
        *
        * @return true on success, false if pausing failed.
        */
        static bool PauseAllSounds();

        /**
        * @brief Pauses all sounds associated with a specific tag.
        *
        * @param tag The tag identifying which sounds to pause.
        * @return true on success, false if pausing failed.
        */
        static bool PauseTag(const std::string& tag);

        /**
        * @brief Resumes a paused SoundClip.
        *
        * @param clip The SoundClip to resume.
        * @return true on success, false if resuming failed.
        */
        static bool ResumeSound(const SoundClip& clip);

        /**
        * @brief Resumes all paused sounds.
        *
        * @return true on success, false if resuming failed.
        */
        static bool ResumeAllSounds();

        /**
        * @brief Resumes all paused sounds associated with a specific tag.
        *
        * @param tag The tag identifying which sounds to resume.
        * @return true on success, false if resuming failed.
        */
        static bool ResumeTag(const std::string& tag);

        /**
        * @brief Stops a specific SoundClip.
        *
        * @param clip The SoundClip to stop.
        * @param fadeOutMS Optional fade-out time in milliseconds.
        * @return true if the sound was stopped successfully, false otherwise.
        */
        static bool StopSound(const SoundClip& clip, Sint64 fadeOutMS = 0);

        /**
        * @brief Stops all currently playing sounds.
        *
        * @param fadeOutMS Optional fade-out time in milliseconds.
        * @return true if all sounds were stopped successfully, false otherwise.
        */
        static bool StopAllSounds(Sint64 fadeOutMS = 0);

        /**
        * @brief Stops all sounds associated with a specific tag.
        *
        * @param tag The tag identifying which sounds to stop.
        * @param fadeOutMS Optional fade-out time in milliseconds.
        * @return true if the sounds were stopped successfully, false otherwise.
        */
        static bool StopTag(const std::string& tag, Sint64 fadeOutMS = 0);

        /**
        * @brief Checks if a SoundClip is currently playing.
        *
        * @param clip The SoundClip to check.
        * @return true if the clip is playing, false otherwise.
        */
        static bool IsPlaying(const SoundClip& clip);

        /**
        * @brief Sets the master volume for all sounds.
        *
        * @param volume Volume value between 0.0 (silent) and 1.0 (full volume).
        * @return true if the volume was set successfully, false otherwise.
        */
        static bool SetMasterVolume(float volume);

        /**
        * @brief Sets the volume for all sounds associated with a specific tag.
        *
        * @param tag The tag identifying which sounds to modify.
        * @param volume Volume value between 0.0 (silent) and 1.0 (full volume).
        * @return true if the tag volume was set successfully, false otherwise.
        */
        static bool SetTagVolume(const std::string& tag, float volume);

        /**
        * @brief Retrieves detailed information about the current state of the SoundManager.
        *
        * The information includes loaded audio, active tracks, and audio devices.
        *
        * @param outInfo Output string that will contain the detailed information.
        * @return true if information was retrieved successfully, false otherwise.
        */
        static bool GetInfo(std::string& outInfo);

    private:
        SoundManager() = default;
        SoundManager(SoundManager&& s) = delete;
        SoundManager operator=(SoundManager & s) = delete;

        IDManager m_trackIDManager;
        struct Audio {
            MIX_Audio* mixAudio = nullptr;
            AudioTrackID audioTrackID{ SDLCORE_INVALID_ID };
            std::unordered_map<SoundClipID, Audio> subSounds;
            bool fireAndForget = false;
            int refCount = 0;// if refCount == 0; object can be deleted

            void IncreaseRefCount() {
                refCount++;
            }

            void DecreaseRefCount() {
                refCount--;
                if (refCount < 0)
                    refCount = 0;
            }

            Audio() = default;
            Audio(MIX_Audio* audio)
                : mixAudio(audio) {}
        };

        struct AudioTrack {
            MIX_Track* track = nullptr;
            float durationMS = 0.0f;
            Sint64 frameCount = 0;
            int frequency = 0;

            float volume = 1.0f;
            Vector2 position;

            std::string tag;
            bool isDeleted = false;
            bool isPlaying = false;

            AudioTrack() = default;
            AudioTrack(MIX_Track* _track, const SoundClip& _clip, const std::string& _tag) 
                : track(_track), durationMS(_clip.GetDurationMS()), 
                frameCount(_clip.GetNumberOfFrames()), frequency(_clip.GetFrequency()), tag(_tag) {}
        };

        // ============== Static ==============

        static bool Init(SDL_AudioDeviceID audio = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
        static void Quit();

        /**
        * @brief Checks if a current valid instance of this class exist
        * @return true if valid instance exists. Call SDLCore::GetError() for more information 
        */
        static bool InstanceExist();

        static bool AddSoundRef(SoundClipID id);
        static bool ReleaseSoundRef(SoundClipID id);

        /*
        * @brief applys params like volume and pitch of clip to its corresponding audiotrack
        */
        static bool ApplyClipParams(const SoundClip& clip);
        static bool ApplyClipParams(AudioTrack* audioTrack, const SoundClip& clip);

        /**
        * @brief Creates a new sound and takes ownership of the given audio.
        *
        * This function is intended to be called internally by the SoundClip class.
        * If a sound with the same ID already exists, it will be replaced:
        * the old audio will be marked as deleted and destroyed.
        *
        * @param id The ID of the sound clip.
        * @param audio Pointer to the MIX_Audio to take ownership of.
        * @return true on success. Call SDLCore::GetError() for more information
        */
        static bool CreateSound(SoundClipID id, MIX_Audio* audio);

        /**
        * @brief Deletes a sound by its ID.
        *
        * This function is intended to be called internally by the SoundClip class.
        * Marks the audio as deleted and frees it when no more references exist.
        *
        * @param id The ID of the sound clip to delete.
        * @return true on success. Call SDLCore::GetError() for more information
        */
        static bool DeleteSound(const SoundClip& clip);

        // ============== Member ==============
        MIX_Mixer* m_mixer = nullptr;
        std::unordered_map<AudioTrackID, AudioTrack> m_audioTracks;// uses audio from m_audios to play sounds
        std::unordered_map<SoundClipID, Audio> m_audios;// audio gets added from clip
        std::unordered_map<SoundClipID, SoundClipID> m_subAudio;// map from sub sound to sound
        std::vector<AudioPlaybackDevice> m_devices;
        IDManager m_deviceIDManager{ 1 };

        void Cleanup();

        /*
        * @brief Creates a new list of AudioPlaybackDevice and deletes the old one
        * @return true on success. Call SDLCore::GetError() for more information
        */
        bool CreateDevices();

        AudioTrack* GetAudioTrack(SoundClipID id, SoundClipID subID);
        AudioTrack* GetAudioTrack(AudioTrackID id);
        Audio* GetAudio(SoundClipID id);

        bool TryGetMixer(MIX_Mixer*& mixer, const std::string& func);

        /**
        * @brief Creates an SDL_PropertiesID for configuring playback parameters of a sound clip.
        *
        * The caller is responsible for destroying the returned properties object with SDL_DestroyProperties()
        * after it is no longer needed. The function initializes the property set with values required by
        * SDL_mixer, such as the loop-count parameter.
        *
        * @param clip Reference to the SoundClip providing playback configuration (e.g. loop count).
        * @return A valid SDL_PropertiesID on success, or 0 on failure.
        */
        SDL_PropertiesID CreateProperty(const SoundClip& clip);
        bool CreateAudioTrack(AudioTrack*& audioTrack, const SoundClip& clip, const std::string& tag);
        void MarkTrackAsDeleted(AudioTrack* audioTrack, AudioTrackID id);
        void OnTrackStopped(AudioTrackID id);
    };

}